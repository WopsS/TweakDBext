# TweakDB format

| Offset              | Name     | Members                   |
|---------------------|----------|---------------------------|
| `00`                | Header   | `magic : int32`           |
|                     |          | `blobVersion : int32`     |
|                     |          | `parserVersion : int32`   |
|                     |          | `recordsChecksum : int32` |
|                     |          | `flatsOffset : int32`     |
|                     |          | `recordsOffset : int32`   |
|                     |          | `queriesOffset : int32`   |
|                     |          | `groupTagsOffset : int32` |
| `flats offset`      |          |                           |
| `records offset`    |          |                           |
| `queries offset`    |          |                           |
| `groupTags offset`  |          |                           |
