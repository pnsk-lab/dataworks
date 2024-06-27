# DWF - DataWorks Database Format 1.0 {#FORMATv1}

DWFはDataWorks用に作られたシンプルなデータベースフォーマットです。

## 指定

`.dwf`を拡張子として使うべきです。 (SHOULD)

すべての数値はビッグエンディアンである必要があります。 (MUST)

### ファイルの構造

最初の3バイトは`7F 44 57` (`\x7F DW`)である必要があります。 (MUST、文書内では`signature`と呼びます)

データベースのエントリー(文書内では`dbentry`と呼びます)はこのフォーマットである必要があります: (MUST)

| 名称 | サイズ | 型 | 補足 |
| ---- | ---- | ---- | ---- |
| flag | 1バイト | uint8\_t | |
| length | 8バイト | uint64\_t | 実データのサイズです。`dbentry`が再利用された際にここが`size`と異なる場合があります。 |
| size | 8バイト | uint64\_t | |
| field\_index | 1バイト| uint8\_t | |
| db\_index | 1バイト | uint8\_t | |
| count | 8バイト | uint64\_t | |
| fragnum | 8バイト | uint64\_t | |
| data | sizeバイト | Binary | |

そして2つのフラグが`dbentry`用に定義されています:
| タイプ | マスク | 補足 |
| ---- | ---- | ---- |
| 使用中 | 0b10000000 | エントリーを使っている時にフラグしてください。 |
| 断片 | 0b01000000 | エントリーが断片な時にフラグしてください。 |
| 未設定 | 0b00100000 | 非推奨です。エントリーをダミーとしてとりあえず使う時にフラグされるはずでした。(現在では別のハンドル方法を用いるため、使う必要はありませんし、使わないでください。) |

インデックスエントリー(文書内では`indexentry`と呼びます)はこのフォーマットである必要があります: (MUST)
| 名称 | サイズ | 型 | 補足 |
| ---- | ---- | ---- | ---- |
| flag | 1バイト | uint8\_t | |
| count | 8バイト | uint64\_t | |
| dbname\_len | 1バイト | uint8\_t | |
| dbname | 256バイト | ASCII | |
| fields | 4096バイト | ASCII | フィールド名をNULで区切ってください。最後のフィールド名には1つの代わりに2つNULを入れてください。このように: `type` `name` `<NUL>` `type` `name` `<NUL>` ... `type` `name` `<NUL>` `<NUL>` |

そして5つの型が`indexentry`用に定義されています:
| 名称 | 文字 | 型 | 補足 |
| ---- | --------- | ---- | ---- |
| 文字列 | `S` | ASCII | |
| 整数 | `I` | int64\_t | |
| 浮動小数点 | `F` | double | |
| 論理 | `L` | uint8\_t | 0なら偽、それ以外なら真と判別してください。 (MUST) |
| ヘルプ | `?` | ASCII | 無視されるべきです。 (SHOULD) |

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
