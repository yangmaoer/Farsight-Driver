=================
===platform_device	-	�豸����˵��
=================
[1]mymodule_init����
    (1)����һ��platform_device���󡢹��������ʹ��
    
    (2)��platform_device���������Դ�����ݣ���platform_driverʹ��

    (3)��platform_device������뵽platform_bus����


[2]mymodule_exit����
    (1)��platform_bus������ж��platform_device����


=================
===platform_driver	-	��������˵��
=================
[1]�ֶ�����platform_driver����

[2]mymodule_init����
	<1>��ʼ��platform_driver����
   		ָ���������豸��ƥ���־	-	ע�⣺Ҫ��platform_device�е�������ͬ

		ָ��������ӵ���š���������ж�ص�ʱ�����Ƿ����豸��ʹ��
	
		����ƥ��ص�����
	
		����ע������
	
	 <2>platform_driver���������������

[3]mymodule_exit(void)
	(1)��platform_bus������ж��platform_driver����

=========
PS��	probe���ܡ�remove˵��
	��1��int myprobe(struct platform_device *platformdev)
		��1��setup_PWM_cdev(platformdev);
     			(1)������Դ���ܽṹ��ָ��
			
   			  (2)��ȡ��Դ
				
   			 (3)����Դ�е������ַת���������ַ

  			  (4)cdev��ʼ��

			  (5)�豸������ƥ����Զ������豸�ڵ�
				
   			(6)PWM��ʼ��

	��2��int myremove(struct platform_device *platformdev)
		��1���ػ������ַ
	
		��2��ɾ���豸�ڵ�
			
		��3��ɾ���豸_��
			
		��4��ɾ��cdev
		
		��5��ע���豸��
			



=================
===app		-	Ӧ�ó���˵��
=================
����ο�
1	-	����������
2	-	�رշ�����
3	-	����~~~
ִ������	
    �� ./app /dev/pwm0 ��



