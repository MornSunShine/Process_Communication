# Process_Communication
These are some examples about communication between processes realized by C++.Main methods include message transfering, shared memory(file mapping) and clipboard.

对于大量的信息，用文件映射来的更有效率，如果是短消息，消息机制则更为方便。以及其中的一些注意事项：
使用SendMessage函数发送LB_ADDSTRING时，lParam不可以为NULL，如果是这样，信息发送行为将不会进行；
为了实现对于同一条信息的多样化响应，可以对wParam参数进行设定，比如说本次实验就定义了四个参数R_ACTION、F_MAPPING、T_MESSAGE、C_BOARD，以实现对WM_SETTEXT的多样化操作；
对于向Edit Control输入内容，默认是不可以换行的，需要将属性Mutilines->true，Want return->true，这样就可以进行回车换行操作；
List Box控件的LB_ADDSTRING操作不能添加多行数据，如果多行，会被合并成一行添加，所以，如果要显示多行数据，显示数据的控件只能是Edit Control，这样才能接收多行数据，也就是连同换行符一起接收；
在为剪贴板操作GlobalAlloc全局堆分配空间，将数据输入到剪贴板之后，需要释放这块内存，不然会造成内存溢出；
对于句柄之类，在使用之后，应该尽可能的使用CloseHandle关闭。
