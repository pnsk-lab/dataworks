# DWF - DataWorks Database Format 1.0 {#FORMATv1}

DWF is a simple database format designed for DataWorks.

## Specification

You SHOULD use `.dwf` for the extension.

All numbers MUST use big-endian.

### File structure

First 3 bytes MUST be 7F 44 57 (\x7F DW) (`signature`)

Database entry (`dbentry`) MUST be in this format:

| Name | Size | Type | Info |
| ---- | ---- | ---- | ---- |
| length | 4 bytes | uint32\_t | Size of the entry |
| size | 4 bytes | uint32\_t | |
| type | 1 byte | ASCII | |
| flag | 1 byte | uint8\_t | |
| field\_index | 1 byte | uint8\_t | |
| db\_index | 1 byte | uint8\_t | |
| count | 8 bytes | uint64\_t | |
| fragnum | 8 bytes | uint64\_t | |
| data | \[size\] bytes | ASCII | |

There are 5 types for `dbentry`:
| Type | Character | Type | Info |
| ---- | --------- | ---- | ---- |
| String | `S` | ASCII | |
| Integer | `I` | uint64\_t | |
| Double | `D` | double | |
| Logical | `L` | uint8\_t | 0 for false, other value for true |
| Help | `?` | ASCII | Should be just ignored |

and 2 flags for `dbentry`:
| Type | Mask | Info |
| ---- | ---- | ---- |
| Used | 0b10000000 | Flag this if you are using the entry. |
| Fragment | 0b01000000 | Flag this if the entry is fragment. |

Index entry (`indexentry`) MUST be in this format:
| Name | Size | Type | Info |
| ---- | ---- | ---- | ---- |
| flag | 1 byte | uint8\_t | |
| count | 8 bytes | uint64\_t | |
| dbname\_len | 1 byte | uint8\_t | |
| dbname | 256 bytes | ASCII | |
| fields | 4096 bytes | ASCII | Separate field names using NUL. Put NUL twice on the last field name. |

There is 1 flag for `indexentry`:
| Type | Mask | Info |
| ---- | ---- | ---- |
| Used | 0b10000000 | Flag this if you are using the entry. |

Info entry (`infoentry`) MUST be in this format:
| Name | Size | Type | Info |
| ---- | ---- | ---- | ---- |
| version | 2 bytes | uint16\_t | MUST be 1 for 1.0 |
`infoentry` IS the only thing which SHOULD be compatible in later format.

File MUST look like this:
| Data | Info |
| ---- | ---- |
| `signature` | |
| `infoentry` | |
| uint64\_t  | Last modified time of the database |
| `indexentry` | 1st one |
| `indexentry` | 2nd one |
| ... | |
| `indexentry` | 256th one |
| `dbentry` | 1st one |
| `dbentry` | 2nd one |
| ... | |
| `dbentry` | nth one |
