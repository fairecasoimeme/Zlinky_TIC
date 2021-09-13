from collections import namedtuple
import re
import argparse
import subprocess
import struct
import binascii
import os
import StringIO

parser = argparse.ArgumentParser(description='Add padding to binary')
parser.add_argument('in_file')
parser.add_argument('out_file', nargs='?')
parser.add_argument('-p', '--padding_bytes', type=int)


def parse_sections(file):
    sections = {}

    Section = namedtuple('Section', ['idx', 'name', 'size', 'vma', 'lma', 'offset', 'align', 'flags'])

    objdump = subprocess.check_output(['arm-none-eabi-objdump', '-h', file])

    section_re = re.compile(r'(?P<idx>[0-9]+)\s'
                            r'(?P<name>.{13,})s*'
                            r'(?P<size>[0-9a-f]{8})\s*'
                            r'(?P<vma>[0-9a-f]{8})\s*'
                            r'(?P<lma>[0-9a-f]{8})\s*'
                            r'(?P<offset>[0-9a-f]{8})\s*'
                            r'(?P<align>[0-9*]*)\s*'
                            r'(?P<flags>[[[\w]*[, [\w]*]*)')

    for match in re.finditer(section_re, objdump):
        sec_dict = match.groupdict()

        sec_dict['idx'] = int(sec_dict['idx'])

        for attr in ['vma', 'lma', 'size', 'offset']:
            sec_dict[attr] = int(sec_dict[attr], 16)

        sec_dict['align'] = eval(sec_dict['align'])

        sections[sec_dict['name']] = Section(**sec_dict)

    return sections

args = parser.parse_args()
padding_len = args.padding_bytes
padding = bytearray(padding_len)
elf_file_name = args.in_file

if args.out_file is None:
    args.out_file = elf_file_name

sections = parse_sections(elf_file_name)

last_section = None
for name, section in sections.iteritems():
    if 'LOAD' in section.flags:
        if last_section is None or section.lma > last_section.lma:
            if section.size > 0:
                last_section = section

last_section_name = last_section.name.rstrip()
last_section_name = r"%s" % (last_section_name)
elf_file_name = r"%s" % (elf_file_name)
print last_section_name

dump_section = subprocess.check_output(['arm-none-eabi-objcopy',
                                        '--dump-section',
                                        '%s=data.bin' % last_section_name,
                                        args.out_file])

with open('data.bin', 'ab') as elf_file_name:
        elf_file_name.write(padding)

subprocess.check_output(['arm-none-eabi-objcopy',
                                        '--update-section',
                                        '%s=data.bin' % last_section_name,
                                        args.in_file,
                                        args.out_file])

