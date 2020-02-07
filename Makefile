treesize:
	gcc treesize.c -o home_size -Wall

clean:
	rm home_size

install: treesize
	sudo cp home_size /usr/bin/
	sudo chown root.root /usr/bin/home_size
	sudo chmod a+s /usr/bin/home_size

uninstall:
	sudo rm -r /usr/bin/home_size
