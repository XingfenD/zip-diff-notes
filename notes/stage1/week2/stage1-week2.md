# Stage1 - Week2

## Target

- ZIP基础结构学习，掌握ZIP的结构与解析流程；

### Detailed

1.

### Deliverables

- ZIP文件结构图示
- ZIP文件解析流程

## Content

### ZIP文件结构图示

![ZIP文件结构图示](./pic/zip文件结构.png)

ZIP文件主要由三部分构成:

1. Local File Header: 每个压缩文件的起始位置，包含文件的元数据信息；
2. Central Directory(CD): 压缩文件的目录，记录了所有文件的元数据信息；
3. End of Central Directory(EOCDR): 压缩文件的结束位置，包含目录的元数据信息。

只考虑非分卷压缩的情况，EOCDR中包含指向Central Directory的偏移量，Central Directory的大小，以及CD条目的项数。

中央目录区包含了所有文件的元数据信息，每个文件的元数据信息由一个Central Directory Entry(CD Entry)表示。每个CD项中含有指向对应Local File Header的偏移量。

本地文件头区包含了每个压缩文件的元数据信息，每个文件的元数据信息由一个Local File Header(LFH)表示。文件的实际数据(压缩或未压缩)紧跟在其对应的LFH后面。

### ZIP文件解析流程

本地文件头区已经包含了每个压缩文件的元数据信息，理论上仅通过读取LFH就能解压出压缩包内的所有文件，因此在解析ZIP文件时就产生了两种解析方式。

- 标准解析模式: 先读取整个Central Directory，然后根据CD项中的偏移量，随机访问到对应文件的LFH，从而获取文件的元数据信息。
- 流式解析模式: 从Local File Header开始，顺序解析每个压缩文件的元数据信息，直到无法读取有效的LFH签名为止。

目前大多数ZIP解析器默认采用标准解析模式，忽略掉没有被CD条目指向的LFH。

#### 标准解析模式

从EOCDR中获取指向Central Directory的偏移量，然后根据CD项中的偏移量，随机访问到对应文件的LFH，从而获取文件的元数据信息。

```mermaid
sequenceDiagram
    participant Parser as 解析器
    participant File as ZIP文件
    participant LFH as 本地文件头
    participant CD as 中央目录
    participant EOCDR as 中央目录结束记录

    Parser->>File: 从文件末尾查找EOCDR签名
    File-->>Parser: 返回EOCDR位置
    Parser->>EOCDR: 读取EOCDR内容
    EOCDR-->>Parser: 返回EOCDR数据
    Parser->>Parser: 提取Central Directory偏移量
    Parser->>File: 跳转到Central Directory位置
    loop 读取所有CD项
        Parser->>CD: 读取一个CentralDirectoryHeader
        CD-->>Parser: 返回CD项数据
        Parser->>Parser: 保存CD项
    end
    loop 根据CD项读取所有LFH
        Parser->>CD: 从CD项获取Local File Header偏移量
        CD-->>Parser: 返回偏移量
        Parser->>File: 跳转到对应的Local File Header位置
        Parser->>LFH: 读取LocalFileHeader
        LFH-->>Parser: 返回LFH数据
        Parser->>Parser: 保存LFH
    end
```

#### 流式解析模式

从Local File Header开始，顺序解析每个压缩文件的元数据信息，直到无法读取有效的LFH签名为止。

```mermaid
sequenceDiagram
    participant Parser as 解析器
    participant File as ZIP文件
    participant LFH as 本地文件头

    Parser->>File: 将文件指针定位到文件开头
    loop 顺序读取Local File Header
        Parser->>LFH: 尝试读取LocalFileHeader
        alt 成功读取LFH签名
            LFH-->>Parser: 返回LocalFileHeader数据
            Parser->>Parser: 保存LFH
            Parser->>File: 继续读取下一个LFH
        else 未找到有效的LFH签名或读取失败
            File-->>Parser: 返回读取失败
            Parser->>Parser: 停止解析
        end
    end
```

## 手工篡改ZIP进行实验

`zip_analyze`目录为一个Zip解析器实例，可以使用两种解析模式读取Zip文件结构。另外该目录下包含两个Zip文件，分别为`zip_demo.zip`和`zip_demo_hacked.zip`。两者的区别在于`zip_demo_hacked.zip`在`zip_demo.zip`的基础上，在LFH区添加了一个额外的文件`hacked.txt`对应的LFH及其数据。具体如下图所示：

![zip_demo文件结构](./pic/zip_demo文件结构.png)

![zip_demo_hacked文件结构](./pic/zip_demo_hacked文件结构.png)
