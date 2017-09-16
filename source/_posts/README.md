---
title: whiteout实验过程
date: 2017-06-22 13:50:20
tags: cnn
---

<script type="text/javascript"
   src="http://cdn.mathjax.org/mathjax/latest/MathJax.js?config=TeX-AMS-MML_HTMLorMML">
</script>

## 简介 ##
实验过程的描述。

<!--more-->
# MNIST#
  
## 初期准备##
以前文件有`test_images`和`train_images`两个文件夹，里面是10000张测试图和60000张训练图。现在我要做交叉验证得到参数。
在生成可实验数据之前，要转化图片，先生成图片目录，这时候要把图片的路径和类标都定下来并分开。
用`filetxt`里面的`read`（源代码是`readfilename.cpp`）生成训练/测试图片的目录和类标文件，用法

	./read 图片文件夹 存放信息的txt文件名
	
利用

	./filetxt/read test_images/  filetxt/test.txt
	./filetxt/read train_images/  filetxt/train.txt
	
用`filetxt`里面的`sep`（源代码是`separate.cpp`）生成指定大小数据量，用法

	./sep 输入文件名 输出文件名 数量
	
利用

	./filetxt/sep filetxt/train.txt filetxt/500/500.txt 500
	./filetxt/sep filetxt/train.txt filetxt/1000/1000.txt 1000
	./filetxt/sep filetxt/train.txt filetxt/3000/3000.txt 3000
	./filetxt/sep filetxt/train.txt filetxt/8000/8000.txt 8000
	./filetxt/sep filetxt/train.txt filetxt/20000/20000.txt 20000
	./filetxt/sep filetxt/train.txt filetxt/50000/50000.txt 50000
	
生成不同样本大小的目录和类标文件，实验也分别在这6个数据量级下开展。

用`filetxt`里面的`sep2`（源代码是`separate2.cpp`）生成每个量级底下的5折交叉验证分组，用法

	./sep 输入文件名
	
利用

	./sep2 500/500.txt
	./sep2 1000/1000.txt
	./sep2 3000/3000.txt
	./sep2 8000/8000.txt
	./sep2 20000/20000.txt
	./sep2 50000/50000.txt
	
生成的`train`和`test`一一对应，将要用它去生成转化数据。

## 转化数据集##
  
编译完成后的`caffe`中有`convert_imageset`能够将图片转化成lmdb数据，lmdb数据能够直接用做caffe训练。
用法

	~/caffe/build/tools/convert_imageset --shuffle --gray 图片文件夹路径 txt文件路径 生成lmdb文件路径
	
例如利用

	~/caffe/build/tools/convert_imageset --shuffle --gray train_images/ filetxt/500/500.txt data/500/500_lmdb
	
去生成。
在`filetxt`下面所有的txt文件都应该代表一个数据集，用bash脚本来实现，先关闭ubuntu的dash

	sudo dpkg-reconfigure dash
	
选择NO就是关闭dash了。接下来转化数据，`trans.sh`如下

	#!/bin/bash

	rm -rf data
	mkdir data;
	a=($(find . -name "*.txt"));
	for i in "${a[@]}"; 
	do
		b=${i/.\/filetxt/data};
		c=${b/.txt/_lmdb};
		d=${i/.\/f/f};
		e=${c##*/};
		f=${c/$e/};
		if [ ! -d "$f" ];then 
			mkdir $f;
		fi
		~/caffe/build/tools/convert_imageset --shuffle --gray train_images/ $d $c
	done
	
使用

	sh trans.sh
	
转化数据。

## 编辑配置文件##
  
配置的网络是lenet的原版再添加上一层全连接层。
有四块实验，分别是基础BP（stdbp），添加drop层（drop），添加shake层（shake），添加white层（white）。
配置文件caffe里面都有。
配置完之后存放在文件夹中，等待调用。

## 交叉验证##
  
这一步是调参的重要步骤。
实验在每一个数据量级下训练都做20次epoch，batchsize都为100。由此在不同的数据量级下，计算出需要的迭代次数，做交叉验证。

$$ epochs*trainsize=iterations*batchsize $$

每一组交叉验证在不同的参数上进行运算，此实验采取的是五折五次交叉验证，即把数据分为5份，每次用其中的4份训练，1份测试，做5次，并且把得到的指标做平均。比较在不同参数下的指标，取最好的指标，作为该组样本的参数。
之前做过为了交叉验证的数据分组，所以现在给出调用方式

	#!/usr/bin/env sh

	rm -rf cv/drop/lenet_train_test.prototxt cv/drop/lenet_train_test.prototxt
	rm -rf cv/drop/500 cv/drop/1000 cv/drop/3000 cv/drop/8000 cv/drop/20000 cv/drop/50000
	mkdir cv/drop/500 cv/drop/1000 cv/drop/3000 cv/drop/8000 cv/drop/20000 cv/drop/50000 #删除接下来要创建，但是上一次运行会存在的文件
	cp cv/drop/lenet_train_test.bak cv/drop/lenet_train_test.prototxt
	cp cv/drop/lenet_solver.bak cv/drop/lenet_solver.prototxt #复制配置文件，以防前一次运行中断导致的文件修改错误
	
	a=($(find . -name "*_train_lmdb")); #找到各种数据集
	for i in "${a[@]}";  #对每一个训练数据集，都进行下面的训练测试过程
	do
		#i ./data/8000/8000_1_train_lmdb
		i=${i/.\/data/.}; 
		b=${i/_train_lmdb/}; # ./8000/8000_1
		e=${b/./cv\/drop}; # cv/drop/8000/8000_1
		c=${i/.\//data\/}; # data/8000/8000_1_train_lmdb
		d=${c/_train_lmdb/_test_lmdb}; # data/8000/8000_1_test_lmdb
		#各种文件名路径的预先生成
		sed -i -e 's|mnist_train_lmdb|'$c'|' cv/drop/lenet_train_test.prototxt
		sed -i -e 's|mnist_test_lmdb|'$d'|' cv/drop/lenet_train_test.prototxt #替换配置文件底下的训练集位置
		f=${i#*\/}; # 8000/8000_1_train_lmdb
		g=${f%\/*}; # 8000
		let h=$g/5; 
		sed -i -e 's|hhhhhhhhhhh|'$e'|' cv/drop/lenet_solver.prototxt
		sed -i -e 's|max_iter: 10000|max_iter: '$h'|' cv/drop/lenet_solver.prototxt
		sed -i -e 's|snapshot: 5000|snapshot: '$h'|' cv/drop/lenet_solver.prototxt #同上
	
		~/caffe/build/tools/compute_image_mean `echo $d` mean.binaryproto #先生成均值文件

		echo trainstart
		echo [${f#*\/}]
		~/caffe/build/tools/caffe train --solver=cv/drop/lenet_solver.prototxt #训练
		echo trainend

		echo teststart
		~/caffe/build/tools/caffe test --model=cv/drop/lenet_train_test.prototxt --weights=`echo $e'_iter_'$h'.caffemodel'` #测试
		echo testend
	
		sed -i -e 's|'$e'|hhhhhhhhhhh|' cv/drop/lenet_solver.prototxt
		sed -i -e 's|max_iter: '$h'|max_iter: 10000|' cv/drop/lenet_solver.prototxt
		sed -i -e 's|snapshot: '$h'|snapshot: 5000|' cv/drop/lenet_solver.prototxt
		sed -i -e 's|'$c'|mnist_train_lmdb|' cv/drop/lenet_train_test.prototxt
		sed -i -e 's|'$d'|mnist_test_lmdb|' cv/drop/lenet_train_test.prototxt #将配置文件修改回去，然后进入下一次循环
	done
	
这个类似的文件，四大类实验都要做，而且CV的话，每一个参数都要做一次全部的工作。现在程序已经能自己在固定参数下，跑完所有训练集一次了。
接下来就要让电脑多跑几组参数。给出方式

	#!/usr/bin/env sh

	jg=0.002;
	st=0;
	for i in {0..20} #i是从0到0.04，间隔为0.002的参数
	do
		c=$(echo "$jg * $i" | bc)
		k=$(echo "$c + $st" | bc) #bash里面的双精度运算
		sed -i -e 's|shakeout_scale: 1|shakeout_scale: '$k'|' cv/shake/lenet_train_test.bak #修改参数
		sh train_shake_cv.sh 2>&1 | tee `echo 'log/shake_'$k'_log.txt'` #进行全数据集的多次实验训练，并写入日志文件
		sed -i -e 's|shakeout_scale: '$k'|shakeout_scale: 1|' cv/shake/lenet_train_test.bak #改回默认参数，以便下次再改再用
		./filetxt/pick `echo 'log/shake_'$k'_log.txt'` `echo 'log/shake_'$k'.txt'` #日志文件的提取关键准确率
	done

上面例子是测试0到0.04间隔为0.002的参数的效果，得到结果之后，再可以往深入测试。

## 选择参数##
交叉验证出来的参数并不是时分有借鉴意义，虽然理论上可行，但是实际上交叉验证得到的参数-正确率曲线是有多个峰值的，还会有平缓的地方

![cv_l1](/md_pic/cv_l1.png "cv_l1")
![cv_l2](/md_pic/cv_l2.png "cv_l2")

这是十分痛苦的结果，都尝试的话，这和每一个参数直接测试训练，又有什么大的区别呢？
感觉为了做数据，需要跑全部，取最好的，这样应该就不会有大的误差，毕竟这些方法类似，最好的正确率也一定符合理论的规则。