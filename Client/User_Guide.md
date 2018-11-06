用户手册
====
使用方法
----
* 病人端使用方法  
运行代码`python patient.py 用户号`  
即可开始采集数据，采集到的数据点，将以CSV文件格式保存，同时心电图形将以PNG格式保存，命名规则为:**test+用户号+编号**，在采集完数据后，客户端将自动将采集到的数据(数据点+图形）打包发送至服务端，以供医生分析您的心电图，及时诊断。

* 医生端使用方法  
运行代码
	`python doctor.py 用户号 文件号`   
即可开始分析病人数据，不输入文件号默认为7 。现阶段的数据分析以对心跳波形的简单判断为主，如：心跳速率、窦性心律不齐的检测等。

How to use
----
* For patient  
Run patient client:`python patient.py usernumber`  
The client will send the data(including the data points and the graphs) to the server automatacally.
* For doctor   
Run doctor client:`python doctor.py usernumber filenumber`   
The client will open the datafile of your patients and you can esaily anaylse the conditions of their heart beats. **The default filenumber is 7.**

注意事项
----
* 由于传感器的原因，病人可以待心跳数据稳定后再打开数据采集的客户端。
* 由于默认数据点文件为文件7。因此，在测量时，请测量超过60秒。
* 由于数据采样频率有限，医生端的数据分析功能仅供参考。
* 如有问题，请联系客服。
