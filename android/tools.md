# 工具

## 查看logcat
- [pidcat](https://github.com/JakeWharton/pidcat.git)

## 反编译apk
###### apktool dex2jar jd-gui
- apktool d -f  test.apk  test  
- dex2jar   classes.dex //变成jar
- 在使用jd-gui看源代码

## 动态调试
## 重打包
- apktool.bat d -o <output_dir> test.apk
- apktool.bat b -o <output.apk> <input_dir>
- jarsigner -verbose -keystore demo.keystore demo.apk demo.keystore
## 组件访问
- drozer 手机也需要安装
-  adb forward tcp:31415 tcp:31415
-  drozer console connect
-  run app.package.attacksurface 
-  run app.activity.info –a (package name)

## 动态调试
- 发现使用ida和android都是非常麻烦啊，有没有好一点的办法呢
- [AndBug](https://github.com/swdunlop/AndBug)
- ​