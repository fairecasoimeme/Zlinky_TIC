    .global s_asAPduPool
    .section    .data.s_asAPduPool,"aw",%progbits
    .align 4
    .type    s_asAPduPool, %object
    .size    s_asAPduPool, 24
s_asAPduPool:
    .global pdum_apduZDP
pdum_apduZDP:
    .long    s_asapduZDPInstances
    .short    0
    .short    100
    .short    5
    .zero     2
    .global pdum_apduZCL
pdum_apduZCL:
    .long    s_asapduZCLInstances
    .short    0
    .short    100
    .short    5
    .zero     2
