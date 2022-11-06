# schtask-bypass

免杀计划任务进行权限维持，过国内主流杀软。 A schtask tool bypass anti-virus

**今天我会整理开机自启动、登陆自启动、定时执行任务三种权限维持的免杀方法。**

思路很简单，参考微软官方的文档写的dll，之前尝试过exe不免杀，但是dll免杀，直接编译即可。

开机启动使用Dll_task_boot，以SYSTEM权限执行计划任务：

rundll32 Dll_task_boot,DllMain filename

![1667714567689](https://user-images.githubusercontent.com/48757788/200157286-7e00113a-486f-4942-affb-d4ad574fbaf8.jpg)


