/* DH_FMT.H
 * Dynamic Hash File Definitions
 * Copyright (c) 2006 Ladybridge Systems, All Rights Reserved
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 * 
 * START-HISTORY:
 * 31 Dec 23 SD launch - prior history suppressed
 * END-HISTORY
 *
 * START-DESCRIPTION:
 *
 * This include file contains the definition of dynamic file format.
 *
 * *************************************************************************
 * *************************************************************************
 * *************** ALL CHANGES MUST BE REFLECTED IN SDCONV.C ***************
 * *************************************************************************
 * *************************************************************************
 *
 * Groups are numbered from 1.
 *
 * All block pointers are stored as byte offsets in version 0 and 1 files
 * and as "group" numbers in version 3 upwards.
 *
 * END-DESCRIPTION
 *
 * START-CODE
 */

#define DH_GROUP_MULTIPLIER 1024
#define DH_MAX_GROUP_SIZE_BYTES (MAX_GROUP_SIZE * DH_GROUP_MULTIPLIER)

/* File header parameters.
   The DHHeaderSize macro should be used to determine the primary and
   overflow subfile header size.
   The AKHeaderSize macro should be used for AK subfiles.                    */

#define DHHeaderSize(version, group_bytes) \
  (((version) == 0) ? 1024 : (group_bytes))
#define AKHeaderSize(version) (((version) == 0) ? 1024 : DH_AK_NODE_SIZE)

#define MAX_INDICES 32
#define AK_BIG_REC_SIZE 3300
#define MAX_AK_NAME_LEN 63 /* Cannot increase without file change */

/* !!FILE_VERSION!!  Also in BP INT$KEYS.H */
#define DH_VERSION 2

/* Block types for primary and overflow subfiles */

#define DHT_DATA 0
#define DHT_BIG_REC 1
#define DHT_ECB 2

/* Data integrity check macros */

#define InvalidGroupAddress(dhf, addr, group_bytes) \
  ((((addr)-dhf->header_bytes) % (group_bytes)) != 0)

/* Addressing macros */

#define GroupOffset(dhf, g) \
  ((((int64)(g - 1)) * dhf->group_size) + dhf->header_bytes)

#define SetFwdLink(dhf, grp)                               \
  (((grp != 0) && (dhf->file_version < 2))                 \
       ? (((grp)-1) * dhf->group_size + dhf->header_bytes) \
       : (grp))
#define GetFwdLink(dhf, lnk)                                 \
  (((lnk != 0) && (dhf->file_version < 2))                   \
       ? ((((lnk)-dhf->header_bytes) / dhf->group_size) + 1) \
       : (lnk))

#define SetAKFwdLink(dhf, n)                                \
  (((n != 0) && (dhf->file_version < 2))                    \
       ? (((n)-1) * DH_AK_NODE_SIZE + dhf->ak_header_bytes) \
       : (n))
#define GetAKFwdLink(dhf, lnk)                                  \
  (((lnk != 0) && (dhf->file_version < 2))                      \
       ? ((((lnk)-dhf->ak_header_bytes) / DH_AK_NODE_SIZE) + 1) \
       : (lnk))

/* Load calculation */

#define DHLoad(loadbytes, grpsize, mod)       \
  (int32_t)((((double)(loadbytes)) * 100.0) / \
            (((double)(grpsize)) * ((double)(mod))))

#define HeaderLoadBytes(h)             \
  (((int64)((h)->params.load_bytes)) | \
   (((int64)((h)->params.extended_load_bytes)) << 32))

/* ======================================================================
   Header for primary and overflow subfiles                               */

typedef struct DH_HEADER DH_HEADER;
struct DH_HEADER {
  int16_t magic;
#define DH_PRIMARY 0x209A
#define DH_OVERFLOW 0x209B
#define DH_INDEX 0x209C
#define DH_CONVERTING 0x209D /* Primary subfile during sdconv */
  int32_t group_size;        /* As bytes */
  struct {
    int32_t modulus; /* Current modulus */
    int32_t min_modulus;
    int32_t big_rec_size;
    int16_t split_load;          /* Percent */
    int16_t merge_load;          /* Percent */
    u_int32_t load_bytes;        /* Bytes (LS32 bits, see below) */
    int32_t mod_value;           /* Imaginary file size for hashing */
    int16_t longest_id;          /* Longest record id in file */
    int16_t extended_load_bytes; /* MS16 bits of 48 bit load bytes value */
    int32_t free_chain;          /* Head of free overflow space */
  } params;
  u_int16_t flags;      /* See DH_FILE structure */
  u_int32_t ak_map;     /* LSB = first AK (AK 0) */
  u_char file_version;  /* DH_VERSION */
  u_char trigger_modes; /* When does trigger fire? */
#define TRG_PRE_WRITE 0x01
#define TRG_PRE_DELETE 0x02
#define TRG_POST_WRITE 0x04
#define TRG_POST_DELETE 0x08
#define TRG_READ 0x10
#define TRG_PRE_CLEAR 0x20
#define TRG_POST_CLEAR 0x40
  /* If this byte is zero but there is a trigger function, it must be an
       old file so the default actions apply.                              */

  char trigger_name[MAX_TRIGGER_NAME_LEN + 1];
  struct FILESTATS stats ALIGN2;     /* File statistics counters 0235 */
  int32_t jnl_fno;                   /* Journalling file number, zero if off */
  char akpath[MAX_PATHNAME_LEN + 1]; /* Null terminated AK directory path */
  int32_t creation_timestamp;        /* sdtime() at creation */
  int64 record_count;                /* Count of records (approximate). This
                                       value is one greater than the actual
                                       count so that we can recognise zero
                                       as meaning that the value has not been
                                       "corrected" from old Q_M releases. */
  u_char pad1;
  u_char hash;       /* Hash type */
  int32_t user_hash; /* Hash code */
  int32_t reserved1;

  /* All subsequent unused bytes in the header can be relied on as being
     zero. This can simplify later additions.                            */
} ALIGN2;
#define DH_HEADER_SIZE ((signed int)(sizeof(DH_HEADER)))

/* ======================================================================
   Header for AK subfile                                                  */

typedef struct DH_AK_HEADER DH_AK_HEADER;
#define AK_CODE_BYTES 512
struct DH_AK_HEADER {
  int16_t magic;          /* DH_INDEX as above */
  u_int16_t flags;        /* Flag bits (Also in BP AK_INFO.H) */
#define AK_ENABLED 0x0001 /* Index update and usage is enabled */
#define AK_RIGHT 0x0002   /* Right justified */
#define AK_NULLS 0x0004   /* Includes null values */
#define AK_MV 0x0008      /* Multi-valued */
#define AK_LSORT 0x0010   /* Keys are sorted left aligned or... */
#define AK_RSORT 0x0020   /* ... sorted left aligned (or unsorted) */
#define AK_NOCASE 0x0040  /* Case insensitive AK */
  int16_t fno;            /* Field number, -1 if I-type index */
  int16_t spare;
  int32_t free_chain; /* Pointer to head of free chain */
  /* Although the following items refer to I-types, the entire
        dictionary record is stored here for all indices, not just I-types */
  int32_t itype_len;                 /* Length of i-type expression */
  int32_t itype_ptr;                 /* Pointer if I-type elsewhere, else 0 */
  u_char itype[AK_CODE_BYTES];       /* Buffer for short i-type */
  char ak_name[MAX_AK_NAME_LEN + 1]; /* Name of AK field */
  int32_t data_creation_timestamp;   /* Creation timestamp of data file */
  char
      collation_map_name[MAX_ID_LEN + 1]; /* Name of collation map or null... */
  char collation_map[256];                /* ...and the actual map */

} ALIGN2;
#define DH_AK_HEADER_SIZE ((signed int)(sizeof(DH_AK_HEADER)))

/* ======================================================================
   Record structure                                                       */

typedef struct DH_RECORD DH_RECORD;
struct DH_RECORD {
  int16_t next;        /* Record size (offset to next record) */
  unsigned char flags; /* Flag word */
#define DH_BIG_REC 0x01
  unsigned char id_len; /* Bytes in id */
  union {
    int32_t data_len; /* Data length for normal record */
    int32_t big_rec;  /* Group address for big record */
  } data;
  char id[1]; /* Id starts here, followed by data */
} ALIGN2;
#define RECORD_HEADER_SIZE (offsetof(DH_RECORD, id))
#define MAX_KEY_LEN 255 /* Because id_len is u_char */

/* ======================================================================
   Header for data group block                                            */

typedef struct DH_BLOCK DH_BLOCK;
struct DH_BLOCK {
  int32_t next;       /* Base address of next group in overflow chain */
  int16_t used_bytes; /* Bytes used including this header. Zero in
                               free primary or overflow block */
  u_char block_type;  /* DHT_DATA */
  u_char pad;
  DH_RECORD record;
} ALIGN2;
#define BLOCK_HEADER_SIZE ((signed int)offsetof(DH_BLOCK, record))

/* ======================================================================
   Header for large record block                                          */

typedef struct DH_BIG_BLOCK DH_BIG_BLOCK;
struct DH_BIG_BLOCK {
  int32_t next;       /* Base address of next group in overflow chain */
  int16_t used_bytes; /* Bytes used including this header. Zero in
                               free block */
  u_char block_type;  /* DHT_BIG_REC */
  u_char pad;
  int32_t data_len; /* Record length (valid in first block only) */
  char data[1];
} ALIGN2;
#define DH_BIG_BLOCK_SIZE ((signed int)(offsetof(DH_BIG_BLOCK, data)))

/* ======================================================================
   B-tree subfile structure, always 4kb blocks                            */

#define DH_AK_NODE_SIZE 4096
#define MAX_CHILD 200
#define AK_FREE_NODE 0
#define AK_INT_NODE 1
#define AK_TERM_NODE 2
#define AK_ITYPE_NODE 3
#define AK_BIGREC_NODE 4

typedef struct DH_FREE_NODE DH_FREE_NODE;
struct DH_FREE_NODE {
  int16_t used_bytes; /* Actually not used in free node */
  u_char node_type;   /* AK_FREE_NODE */
  u_char spare;
  int32_t next; /* Pointer to next node in free list */
} ALIGN2;
#define DH_FREE_NODE_SIZE ((signed int)(sizeof(DH_FREE_NODE)))

typedef struct DH_INT_NODE DH_INT_NODE;
struct DH_INT_NODE {
  int16_t used_bytes;        /* Bytes used in node */
  u_char node_type;          /* AK_INT_NODE */
  u_char child_count;        /* Number of child nodes */
  int32_t child[MAX_CHILD];  /* Child node pointers... */
  u_char key_len[MAX_CHILD]; /* ...and length of keys */
  char keys[1];              /* Key list (undelimited) */
} ALIGN2;
#define INT_NODE_HEADER_SIZE (offsetof(DH_INT_NODE, keys))

typedef struct DH_TERM_NODE DH_TERM_NODE;
struct DH_TERM_NODE {
  int16_t used_bytes; /* Bytes used in node */
  u_char node_type;   /* AK_TERM_NODE */
  u_char spare;
  int32_t left;  /* Pointer to left terminal node */
  int32_t right; /* Pointer to right terminal node */
  DH_RECORD record;
} ALIGN2;
#define TERM_NODE_HEADER_SIZE ((signed int)offsetof(DH_TERM_NODE, record))

typedef struct DH_ITYPE_NODE DH_ITYPE_NODE;
struct DH_ITYPE_NODE {
  int16_t used_bytes;
  u_char node_type; /* AK_ITYPE_NODE */
  u_char spare;
  int32_t next;
  u_char data[1];
} ALIGN2;
#define DH_ITYPE_NODE_DATA_OFFSET ((signed int)offsetof(DH_ITYPE_NODE, data))

typedef struct DH_BIG_NODE DH_BIG_NODE;
struct DH_BIG_NODE {
  int16_t used_bytes;
  u_char node_type; /* AK_BIGREC_NODE */
  u_char spare;
  int32_t next;     /* Pointer to next block in chain */
  int32_t data_len; /* Record length (valid in first block only) */
  char data[1];
} ALIGN2;
#define DH_AK_BIG_NODE_SIZE ((signed int)(offsetof(DH_BIG_NODE, data)))

/* END-CODE */
