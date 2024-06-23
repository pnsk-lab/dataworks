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
| flag | 1 byte | uint8\_t | |
| length | 8 bytes | uint64\_t | Size of the actual data inside |
| size | 8 bytes | uint64\_t | |
| field\_index | 1 byte | uint8\_t | |
| db\_index | 1 byte | uint8\_t | |
| count | 8 bytes | uint64\_t | |
| fragnum | 8 bytes | uint64\_t | |
| data | \[size\] bytes | Binary | |

and 2 flags for `dbentry`:
| Type | Mask | Info |
| ---- | ---- | ---- |
| Used | 0b10000000 | Flag this if you are using the entry. |
| Fragment | 0b01000000 | Flag this if the entry is fragment. |
| Unset | 0b00100000 | Flag this if the entry is unset. |

Index entry (`indexentry`) MUST be in this format:
| Name | Size | Type | Info |
| ---- | ---- | ---- | ---- |
| flag | 1 byte | uint8\_t | |
| count | 8 bytes | uint64\_t | |
| dbname\_len | 1 byte | uint8\_t | |
| dbname | 256 bytes | ASCII | |
| fields | 4096 bytes | ASCII | Separate field names using NUL. Put NUL twice on the last field name. Like: `type` `name` `<NUL>` `type` `name` `<NUL>` ... `type` `name` `<NUL>` `<NUL>` |

There are 5 types for `indexentry`:
| Type | Character | Type | Info |
| ---- | --------- | ---- | ---- |
| String | `S` | ASCII | |
| Integer | `I` | int64\_t | |
| Floating | `F` | double | |
| Logical | `L` | uint8\_t | 0 for false, other value for true |
| Help | `?` | ASCII | Should be just ignored |

There is 1 flag for `indexentry`:
| Type | Mask | Info |
| ---- | ---- | ---- |
| Used | 0b10000000 | Flag this if you are using the entry. |

Info entry (`infoentry`) MUST be in this format:
| Name | Size | Type | Info |
| ---- | ---- | ---- | ---- |
| version | 2 bytes | uint16\_t | MUST be 1 for 1.0 |
| mtime | 8 bytes | int64\_t | Last modified time of the database |

`infoentry` IS the only thing which SHOULD be compatible in later format.

File MUST look like this:
| Data | Info |
| ---- | ---- |
| `signature` | |
| `infoentry` | |
| `indexentry` | 1st one |
| `indexentry` | 2nd one |
| ... | |
| `indexentry` | 256th one |
| `dbentry` | 1st one |
| `dbentry` | 2nd one |
| ... | |
| `dbentry` | nth one |
