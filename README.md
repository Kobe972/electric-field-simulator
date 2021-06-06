# electric-field-simulator
这是一个电场线模拟器，支持含有至多一个不带电导体圆片和任意多个点电荷的电场。  
使用方法：在exe文件目录下建立input.txt，每组数据分为两行，第一行为元素类型，可输入charge(点电荷)或circle(导体圆片)，第二行为位置等信息。
对于点电荷，第二行输入三个数，分别是x坐标，y坐标，电荷量，它们直接空格隔开，其中坐标原点在窗口左上角，x坐标范围为0到800，y坐标范围为0到600。对于导体圆片，三个数分别为横纵坐标、圆片半径。注意input.exe最后不能有多余的空行，即使这一行没有任何字符。  
注意：exe文件必须和GUI文件夹放在一个目录里。
