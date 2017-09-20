=================
===platform_device	-	设备部分说明
=================
[1]mymodule_init（）
    (1)申请一个platform_device对象、供驱动框架使用
    
    (2)给platform_device对象添加资源（数据）供platform_driver使用

    (3)把platform_device对象加入到platform_bus总线


[2]mymodule_exit（）
    (1)从platform_bus总线中卸载platform_device对象


=================
===platform_driver	-	驱动部分说明
=================
[1]手动创建platform_driver对象

[2]mymodule_init（）
	<1>初始化platform_driver对象
   		指明驱动和设备的匹配标志	-	注意：要与platform_device中的名字相同

		指明驱动的拥有着、便于驱动卸载的时候检查是否还有设备在使用
	
		声明匹配回调函数
	
		声明注销函数
	
	 <2>platform_driver驱动对象加入总线

[3]mymodule_exit(void)
	(1)从platform_bus总线中卸载platform_driver对象

=========
PS：	probe功能、remove说明
	【1】int myprobe(struct platform_device *platformdev)
		【1】setup_PWM_cdev(platformdev);
     			(1)声明资源接受结构体指针
			
   			  (2)获取资源
				
   			 (3)把资源中的物理地址转化成虚拟地址

  			  (4)cdev初始化

			  (5)设备与驱动匹配后、自动创建设备节点
				
   			(6)PWM初始化

	【2】int myremove(struct platform_device *platformdev)
		（1）回还虚拟地址
	
		（2）删除设备节点
			
		（3）删除设备_类
			
		（4）删除cdev
		
		（5）注销设备号
			



=================
===app		-	应用程序说明
=================
输入参考
1	-	开启蜂鸣器
2	-	关闭蜂鸣器
3	-	唱歌~~~
执行命令	
    《 ./app /dev/pwm0 》



