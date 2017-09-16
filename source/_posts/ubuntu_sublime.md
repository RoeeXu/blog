---
title: ubuntu 下 sublime text 3 的配置
date: 2017-09-15 10:45:42
tags: sublime
---



## 简介
在linux底下，C++ IDE一直都是一个麻烦的问题，不管你是用Emacs、Eclipse、Code::Blocks、VS，还是大神们直接用的vim、nano。要不是调试困难，要不就是IDE容量太大，还有颜色标记的问题。
我更倾向于用轻量级的IDE，在有一定的基础下，我选择用sublime。

<!--more-->

## 安装 sublime text 3

	sudo add-apt-repository ppa:webupd8team/sublime-text-3
	sudo apt-get update 
	sudo apt-get install sublime-text-installer
	
接下来按`win`搜索`sublime`就可以找到刚安装的sublime了。

## 安装 package control
打开sublime，``Ctrl+ ` ``打开控制台，输入在[https://packagecontrol.io/installation](https://packagecontrol.io/installation)中的内容
	
	import urllib.request,os,hashlib; h = '6f4c264a24d933ce70df5dedcf1dcaee' + 'ebe013ee18cced0ef93d5f746d80ef60'; pf = 'Package Control.sublime-package'; ipp = sublime.installed_packages_path(); urllib.request.install_opener( urllib.request.build_opener( urllib.request.ProxyHandler()) ); by = urllib.request.urlopen( 'http://packagecontrol.io/' + pf.replace(' ', '%20')).read(); dh = hashlib.sha256(by).hexdigest(); print('Error validating download (got %s instead of %s), please try manual install' % (dh, h)) if dh != h else open(os.path.join( ipp, pf), 'wb' ).write(by) 
	
以上内容可能有变化，所以进入官网查看最好。

## 安装 SublimeClang
	
	sudo apt-get install cmake build-essential clang git
	cd ~/.config/sublime-text-3/Packages
	git clone --recursive https://github.com/quarnster/SublimeClang SublimeClang
	cd SublimeClang
	cp /usr/lib/x86_64-linux-gnu/libclang-3.8.so.1 internals/libclang.so      #如果你的clang库不是3.8版本的话，请将对应版本的库拷贝到internals中
	cd src
	mkdir build
	cd build
	cmake ..
	make
	
然后用`sudo subl`打开sublime。`ctrl+shift+p`打开package control，搜索`sublimeclang settings`，然后选择带User那一行，在打开的文件中输入如下信息：

	{
		"show_output_panel": false,
		"dont_prepend_clang_includes": true,
		"inhibit_sublime_completions": false,
		
		"options":
		 [
		 	"-std=gnu++11",
		 	"-isystem", "/usr/include",
		 	"-isystem", "/usr/include/c++/*",
		 	"-isystem", "/usr/include/c++/5",
		 	"-isystem", "/usr/include/c++/5/*",
		 	"-isystem", "/usr/include/boost",
		 	"-isystem", "/usr/include/boost/**",
		 	"-isystem", "/usr/lib/gcc/x86_64-linux-gnu/5/include",
		 	"-isystem", "/usr/lib/gcc/x86_64-linux-gnu/5/include/*"
		 ]
	}
	
我的gcc版本为5，如果你的不是请替换对应的版本。
至此，恭喜安装完了，以后打开`.c`或者`.cpp`文件，`#include`相应文件后，sublime都会自动提示函数和变量名称，在保存的同时也会检查语法错误。
运行就在teminal中`g++ -o 1 1.cpp`，然后运行`./1`好了。
缺点，不能调试。
	
## 设置输入中文

到目录`/opt/sublime_text`下，创建文件`sublime_imfix.c`，写入如下内容：

	/*
	sublime-imfix.c
	Use LD_PRELOAD to interpose some function to fix sublime input method support for linux.
	By Cjacker Huang <jianzhong.huang at i-soft.com.cn>
	gcc -shared -o libsublime-imfix.so sublime_imfix.c  `pkg-config --libs --cflags gtk+-2.0` -fPIC
	LD_PRELOAD=./libsublime-imfix.so sublime_text
	*/
	#include <gtk/gtk.h>
	#include <gdk/gdkx.h>
	typedef GdkSegment GdkRegionBox;
	struct _GdkRegion
	{
	  long size;
	  long numRects;
	  GdkRegionBox *rects;
	  GdkRegionBox extents;
	};
	GtkIMContext *local_context;
	void
	gdk_region_get_clipbox (const GdkRegion *region,
	            GdkRectangle    *rectangle)
	{
	  g_return_if_fail (region != NULL);
	  g_return_if_fail (rectangle != NULL);
	  rectangle->x = region->extents.x1;
	  rectangle->y = region->extents.y1;
	  rectangle->width = region->extents.x2 - region->extents.x1;
	  rectangle->height = region->extents.y2 - region->extents.y1;
	  GdkRectangle rect;
	  rect.x = rectangle->x;
	  rect.y = rectangle->y;
	  rect.width = 0;
	  rect.height = rectangle->height; 
	  //The caret width is 2; 
	  //Maybe sometimes we will make a mistake, but for most of the time, it should be the caret.
	  if(rectangle->width == 2 && GTK_IS_IM_CONTEXT(local_context)) {
	        gtk_im_context_set_cursor_location(local_context, rectangle);
	  }
	}
	//this is needed, for example, if you input something in file dialog and return back the edit area
	//context will lost, so here we set it again.
	static GdkFilterReturn event_filter (GdkXEvent *xevent, GdkEvent *event, gpointer im_context)
	{
	    XEvent *xev = (XEvent *)xevent;
	    if(xev->type == KeyRelease && GTK_IS_IM_CONTEXT(im_context)) {
	       GdkWindow * win = g_object_get_data(G_OBJECT(im_context),"window");
	       if(GDK_IS_WINDOW(win))
	         gtk_im_context_set_client_window(im_context, win);
	    }
	    return GDK_FILTER_CONTINUE;
	}
	void gtk_im_context_set_client_window (GtkIMContext *context,
	          GdkWindow    *window)
	{
	  GtkIMContextClass *klass;
	  g_return_if_fail (GTK_IS_IM_CONTEXT (context));
	  klass = GTK_IM_CONTEXT_GET_CLASS (context);
	  if (klass->set_client_window)
	    klass->set_client_window (context, window);
	  if(!GDK_IS_WINDOW (window))
	    return;
	  g_object_set_data(G_OBJECT(context),"window",window);
	  int width = gdk_window_get_width(window);
	  int height = gdk_window_get_height(window);
	  if(width != 0 && height !=0) {
	    gtk_im_context_focus_in(context);
	    local_context = context;
	  }
	  gdk_window_add_filter (window, event_filter, context); 
	}
	

安装

	sudo apt-get install build-essential
	sudo apt-get install libgtk2.0-dev
	

编译共享内库
	
	sudo gcc -shared -o libsublime_imfix.so sublime_imfix.c `pkg-config --libs --cflags gtk+-2.0` -fPIC


设置 LD_PRELOAD 并启动 Sublime Text

	LD_PRELOAD=./libsublime_imfix.so subl


修改`/usr/share/applications/sublime_text.desktop`为，其中`[...]`的内容不要更改

	[Desktop Entry]
	[...]
	Exec=env LD_PRELOAD=/opt/sublime_text/libsublime_imfix.so /opt/sublime_text/sublime_text %F
	[...]

	[Desktop Action Window]
	[...]
	Exec=env LD_PRELOAD=/opt/sublime_text/libsublime_imfix.so /opt/sublime_text/sublime_text -n
	[...]

	[Desktop Action Document]
	[...]
	Exec=env LD_PRELOAD=/opt/sublime_text/libsublime_imfix.so /opt/sublime_text/sublime_text --command new_file
	[...]


修改`/usr/bin/subl`为

	#!/bin/sh
	export LD_PRELOAD=/opt/sublime_text/libsublime_imfix.so
	exec /opt/sublime_text/sublime_text "$@"
	

再次打开sublime就可以输入中文了，也就可以中文注释了。
  
