# QtFrozenColumnExample
Frozen Column Example，qt固定表格第一列，固定树控件第一列
代码来自于QT官方例子

##Frozen Column Example

![example](https://github.com/ooklasd/QtFrozenColumnExample/blob/master/example1.png)

We use Qt's model/view framework to implement a table with its first column frozen. This technique can be aplied to several columns or rows, as long as they are on the edge of the table.
The model/view framework allows for one model to be displayed in different ways using multiple views. For this example, we use two views on the same model - two table views sharing one model. The frozen column is a child of the main tableview, and we provide the desired visual effect using an overlay technique which will be described step by step in the coming sections.

![example2](https://github.com/ooklasd/QtFrozenColumnExample/blob/master/example2.png)


