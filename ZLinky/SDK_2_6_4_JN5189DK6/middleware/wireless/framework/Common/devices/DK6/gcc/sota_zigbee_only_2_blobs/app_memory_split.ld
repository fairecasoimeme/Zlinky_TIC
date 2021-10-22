/* 
* Merory split for a Zigbee application
* 2 blobs split
*/

/* ---------------------------------------------
		BLOB Position in flash
m_blob_position1 - APP blob
m_blob_position2 - Zigbee stack blob
--------------------------------------------- */

/* --------------- FLASH ----------------------*/
m_blob1_size            = 0x3BC00; /* 244736 bytes - APP blob */
m_blob2_size            = 0x1C800; /* 116736 bytes - Zigbee stack blob */


/* --------------- RAM0 ----------------------*/
m_blob1_RAM0_size       = 0x12965; /* 76133 bytes - APP blob */
m_blob2_RAM0_size       = 0x22BB; /* 8891 bytes - Zigbee stack blob */