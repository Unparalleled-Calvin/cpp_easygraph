## Boost Python 使用

### Windows

#### 安装

确保VS已安装MSVC编译器和Win10 SDK

![image-20220711004442554](https://s2.loli.net/2022/07/11/Y1I3Pg6fcwpRhx5.png)

在VS的Develop Command Prompt终端中进入下载好的Boost Python文件夹

注意：

- 编译出的lib库与当前环境下的python是匹配的，所以如果要为anaconda下某个python编译，需要在Develop Command Prompt中conda activate ...

运行bootstrap.bat编译出b2.exe。然后根据本机环境修改如下命令并运行：

```cmd
b2 install --toolset=msvc-14.3 --with-python --prefix="D:\Program Files (x86)\Boost" link=static runtime-link=shared threading=multi address-model=64
```

注意：
- 这里toolset的msvc版本需要对应自己安装VS的msvc版本，我这里是14.3
- prefix是编译后文件的存储路径
- link选择static
- address-model选择与python版本对应的，例如64为python就选择64

#### 使用

##### 依赖配置

项目属性

- 常规
  - 配置类型：动态库(.dll)
  - ![image-20220711004156641](https://s2.loli.net/2022/07/11/wUQfeBIpAjlJgMC.png)
- 高级
  - 目标文件拓展名：.pyd
  - ![image-20220711004259682](https://s2.loli.net/2022/07/11/QnCsOJTBM9Wbo5m.png)
- VC++目录
  - 包含目录：目标环境下python的include文件夹，上述b2编译生成路径中的include文件夹
  - 库目录：目标环境下python的libs文件夹，上述b2编译生成路径中的lib文件夹
  - ![image-20220711004047611](https://s2.loli.net/2022/07/11/7CxRNuLj3To1fV9.png)

##### 代码编写

boost::python::extract\<T>(...)  负责从参数中提取出类型T，这是常用的类型转换方式

其余请参考Wiki、各问答网站、及博客，另外也可参考Bilibili上视频教程

注意：
- 每个头文件中都应写上`#define BOOST_PYTHON_STATIC_LIB`，否则链接时会出现找不到python_xxx.lib【b2编译阶段选择link=shared才会生成】的报错

##### 生成使用

直接点击生成解决方案即可。确认生成目录下成功生成.pyd文件后尝试import。