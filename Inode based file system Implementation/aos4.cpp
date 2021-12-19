/************************** HEADER FILES ***********************************/
#include<iostream>
#include<stdlib.h>
#include<stdio.h>
#include<unordered_map>
#include<vector>
#include<fstream>
#include<iostream>
#include<map>
#include<ctime>
#include<cstdio>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>
#include<bits/stdc++.h>

using namespace std;


/************************* DEFINES ****************************************/ 
#define BLOCK_SIZE 4096
#define DISK_BLOCK 128000 //500MB
#define ll long long int
#define NO_OF_INODES 76800    //60% of 128000   
#define NO_OF_FILE_DESCRIPTORS 32 


struct inode_free_bitmap{
	bool bitmap[76800];
};

struct total_block_free_bitmap{
	bool bitmap[DISK_BLOCK];
};

/* inode field */
struct inode{
	//can this file be read/written/executed?
	char mode[4] ;
	//size of the file
	ll size;
	//how many blocks are allocated for this file
	int blocks;
	//disk pointer only direct block pointer
	int block_pointer[12];
};
struct inode_table{
	unordered_map<string,inode*> file_name_to_inode;
};

struct super_block{

		int no_of_block_for_superblock = ceil(((float)sizeof(super_block)) / BLOCK_SIZE);

		int no_of_block_for_inode_bitmap = ceil(((float)sizeof(inode_free_bitmap)) / BLOCK_SIZE);
		
		int no_of_block_for_data_block_bitmap = ceil(((float)sizeof(total_block_free_bitmap)) / BLOCK_SIZE);
		
	    	int no_of_block_for_inode_table = ceil(((float)sizeof(inode_table)) / BLOCK_SIZE);
	    	
	    	int starting_index_of_inode_bitmap = no_of_block_for_superblock;
	    	
	    	int starting_index_of_data_block_bitmap = no_of_block_for_superblock + no_of_block_for_inode_bitmap;
		
	    	int starting_index_of_inode_table = no_of_block_for_superblock + no_of_block_for_inode_bitmap +
	    	no_of_block_for_data_block_bitmap;
	    	
		int starting_index_of_data_block = no_of_block_for_superblock + no_of_block_for_inode_bitmap +
	    	no_of_block_for_data_block_bitmap + no_of_block_for_inode_table ;
	    	
	   
	    	int total_no_of_available_blocks = DISK_BLOCK - starting_index_of_data_block;

	   
};



class Disk{
	public:
		char disk_name[100];
		vector<string>files;
		//vector<string>file;
		//int var;
		bool offset[25] ; //each file is 20mb, no of files at a time=500/20
		unordered_map<int,pair<string,string>> fd_to_file_mode;
		unordered_map<string,pair<int,ll> >file_to_off_size;
		//FILE *disk_ptr;
		Disk(char *s){
			strcpy(disk_name,s);
			FILE* disk_ptr = fopen(disk_name,"wb");
			char buffer[4096];
			memset(buffer,0,4096*sizeof(char));
			for(int i=0 ;i<DISK_BLOCK ;i++){
				fwrite(buffer,sizeof(char),sizeof(buffer),disk_ptr);
			}	
			fclose(disk_ptr);
			//var =5;
			for(int i=0;i<25;i++){
				offset[i] = 0;
			}
		}
		
		
		
};

unordered_map<string,Disk*> disk;
vector<bool>file_descriptor(32,0);
string mounted_disk;
//Disk *abc ;
/************************* FUNCTION *****************************************/
string create_disk(char *name){
	for(auto it = disk.begin();it!=disk.end();++it){
		if(it->first == string(name)){
			return "Disk already exist in this name";
		}
	}
	//abc= new 
	disk[name] = new Disk(name);
	//cout<<disk[name]->disk_name<<endl;
	//cout<<disk[name]->var<<endl;
	//cout<<disk[name]<<endl;
	//cout<<disk[name]->files.size()<<endl;
	
	return "Disk created successfully.";
		
}

int mount_disk(char *name){
	for(auto it = disk.begin();it != disk.end();++it){
		if(it->first == string(name)){
			//memset(mounted_disk,0,100*sizeof(char));
			//strcpy(mounted_disk,it->first);
			mounted_disk = "";
			mounted_disk = name;
			//cout<<it->first<<" "<<name<<" " <<mounted_disk<<endl;
			//cout<<strlen(it->first)<<" "<<strlen(name)<<" "<<strlen(mounted_disk)<<endl;
			//cout<<abc<<endl;
			//cout<<abc->disk_name<<endl;
	//cout<<disk[name]->var<<endl;
	
	//cout<<disk[mounted_disk]->files.size()<<endl;
			return 1;
		}
	}
	return 0;
	
}

void unmount_disk(){
	for(int i=0;i<32;i++){
		file_descriptor[i] = 0;
	}
	Disk *d = disk[mounted_disk];
	d->fd_to_file_mode.clear();
	mounted_disk = "";
}
void create_file(char *name){
			Disk* d = disk[mounted_disk];
			int i;
			//cout<<"first loop2"<<endl;
			//cout<<disk[mounted_disk]->files.size()<<endl;
			for(int j=0;j<(d->files.size());j++){
				//cout<<"for first loop"<<endl;
				//cout<<d->files[j]<<endl;
				if(d->files[j] == string(name)){
					//cout<<d->files[j]<<endl;
					cout<<"File name already exist"<<endl;
					return ;
				}
			}
			//cout<<"first loop2"<<endl;
			for(i=0;i<25;i++){
				if(d->offset[i] == 0)
				{
					break;
				}
			}
			//cout<<"first loop"<<endl;
			if(i !=25){
				d->files.push_back(name);
				d->offset[i] = 1;
				d->file_to_off_size[name] = make_pair(i,0);
				cout<<"File created successfully."<<endl;
				return ;
			}
			//cout<<"first loop"<<endl;
			cout<<"Disk is full."<<endl;;
}

int open_file(char *fn, char mode[]){
	int i;
	for(i=0;i<32;i++){
		if(file_descriptor[i] == 0){
			file_descriptor[i] = 1;
			break;
		}
	}
	if(i == 32){
		cout<<"File descriptor is not available"<<endl;
		return -1;
	}
	int fd =i;
	Disk* d =disk[mounted_disk];
	for(int j=0;j<(d->files.size());j++){
				if(d->files[j]==string(fn)){
					d->fd_to_file_mode[fd] = make_pair(fn,mode);
					return fd;
					//break;
				}
	}
	cout<<"File does not exist"<<endl;
	return -1;			

}
void read_file(int fd){
	int i;
	int flag =0;
	for(i=0;i<32;i++){
		if(file_descriptor[i] == 1 && fd == i){
			flag =1;
			
		}
	}
	if(flag==1){
		Disk* d =disk[mounted_disk];
		for(auto it= d->fd_to_file_mode.begin();it!= d->fd_to_file_mode.end();++it){
				if(it->first == fd){
					string file_name = d->fd_to_file_mode[fd].first;
					string mode = d->fd_to_file_mode[fd].second;
					int off = d->file_to_off_size[file_name].first;
					ll file_size = d->file_to_off_size[file_name].second;
					
					if(mode == "r"){
						FILE *dp = fopen(d->disk_name,"r");
						
						ll file_size = d->file_to_off_size[file_name].second;
						fseek(dp,off*20971520,SEEK_SET);
						cout<<"your File content: "<<endl;
						//cout<<"off "<<off<<" file_size "<<file_size<<endl;
						ll temp =0;
						
						 do
						   {							
							int c = fgetc(dp);
						 							
							if (temp==file_size)
							    break ;
						 
							printf("%c", c);
							temp++;
						    }while(1);
 
						
						fseek(dp,0,SEEK_SET);
						fclose(dp);
						
						//pwrite(dp,buffer,strlen(buffer),off*20971520)
					}
					else{
						cout<<"you dont have read permission"<<endl; 
					}
					break;
				}
			}
			//cout<<"File not found"<<endl;;
	
	}
	else{
	cout<< "fd does not exist"<<endl;
	}
	
}
void write_file(int fd){
	int i;
	int flag =0;
	for(i=0;i<32;i++){
		if(file_descriptor[i] == 1 && fd == i){
			flag =1;
			
		}
	}
	if(flag==1){
		Disk* d =disk[mounted_disk];
		for(auto it= d->fd_to_file_mode.begin();it!= d->fd_to_file_mode.end();++it){
				if(it->first == fd){
					string file_name = d->fd_to_file_mode[fd].first;
					string mode = d->fd_to_file_mode[fd].second;
					int off = d->file_to_off_size[file_name].first;
					ll file_size = d->file_to_off_size[file_name].second;
					if(mode =="r"){
						cout<< "You dont have write permission for this file"<<endl;
						return;
					}
					else if(mode == "a"){
						int dp = open(d->disk_name,O_RDWR);
						
						ll file_size = d->file_to_off_size[file_name].second;
						//fseek(dp,off*20971520+file_size,SEEK_SET);
						cout<<"Enter your content: "<<endl;
						//cout<<"off "<<off<<" file_size "<<file_size<<endl;
						int ch;
						char buffer[4096];
						string temp ="";
						ll fs = file_size;
						while((ch = getchar())!='$'){
							if(file_size == 20971520){
								break;
							}
							//fputc(ch,dp);
							temp += ch;
							file_size++;
						}
						
						strcpy(buffer,temp.substr(1).c_str());
						
						pwrite(dp,buffer,strlen(buffer),off*20971520+fs);
						d->file_to_off_size[file_name].second = file_size;
						//fseek(dp,0,SEEK_SET);
						//fclose(dp);
						close(dp);
						cout<< "File append has been done successfully"<<endl;
						return;
						
					}
					else if(mode == "w"){
						int dp = open(d->disk_name,O_RDWR);
						//fseek(dp,off*20971520,SEEK_SET);
						ll file_size =0;
						cout<<"Enter your content: "<<endl;
						//cout<<"off "<<off<<" file_size "<<file_size<<endl;
						/*int ch;
						while((ch = getchar())!='$'){
							if(file_size == 20971520){
								break;
							}
							fputc(ch,dp);
							file_size++;
						}
						d->file_to_off_size[file_name].second = file_size;
						fseek(dp,0,SEEK_SET);
						fclose(dp);*/
						int ch;
						char buffer[4096];
						string temp ="";
						ll fs = file_size;
						while((ch = getchar())!='$'){
							if(file_size == 20971520){
								break;
							}
							//fputc(ch,dp);
							temp += ch;
							file_size++;
						}
						
						strcpy(buffer,temp.substr(1).c_str());
						
						pwrite(dp,buffer,strlen(buffer),off*20971520+fs);
						d->file_to_off_size[file_name].second = file_size;
						//fseek(dp,0,SEEK_SET);
						//fclose(dp);
						close(dp);
						cout<<"File write has been done successfully"<<endl;
						return ;
					}
					else{
						cout<<"Invalid mode in write"<<endl;
					}
				}
				break;
			}
			//cout<< "File descriptor does not exist.";
	}
	else{
	cout<< "fd does not exist"<<endl;
	}
	

}
void close_file(int fd){
	int i;
	for(i=0;i<32;i++){
		if(file_descriptor[i] == 1 && fd == i){
			file_descriptor[i] = 0;
			Disk* d = disk[mounted_disk];
			for(auto it= d->fd_to_file_mode.begin();it != d->fd_to_file_mode.end();++it){
				if(it->first == fd){
					d->fd_to_file_mode.erase(it);
					break;
				}
			}
			cout<<"File closed."<<endl;
			return;
		}
	}
	
	cout<<"fd does not exist"<<endl;
}
/*void delete_file(int fd){
	int i;
	for(i=0;i<32;i++){
		if(file_descriptor[i] == 1 && fd == i){
			file_descriptor[i] = 0;
			Disk *d = disk[mounted_disk];
			string fn = d->fd_to_file_mode[fd].first;
			d->fd_to_file_mode.erase(fd);
			int off = d->file_to_off_size[fn].first;
			d->offset[off] = 0;
			d->file_to_off_size.erase(fn);
			for(auto it = d->files.begin();it != d->files.end();++it){
				if(fn == *it){
					d->files.erase(it);
					break;
				}
			}
			cout<<"File deleted."<<endl;
			return;
		}
	}
	
	cout<<"fd does not exist"<<endl;
}*/
void delete_file(string file_name){
	int i;
	Disk *d = disk[mounted_disk];
	//unordered_map<int,pair<string,string>> fd_to_file_mode;
	for(auto it = d->fd_to_file_mode.begin();it != d->fd_to_file_mode.end();++it){
		if(it->second.first == file_name){
			cout<<"File is still opened.";
			return ;
		}
	}
	for(string str: d->files){
		if(str == file_name){
			d->files.erase(find(d->files.begin(),d->files.end(),str));
			int off = d->file_to_off_size[file_name].first;
			d->offset[off] = 0;
			d->file_to_off_size.erase(file_name);
		
			cout<<"File deleted."<<endl;
			return;
		}
	}
	
	cout<<"file name does not exist."<<endl;
}
void list_of_files(){

	Disk *d = disk[mounted_disk];
	if(d->files.empty()){
		cout<<"There is no file in the disk"<<endl;
		return;
	}
	cout<<"Files in the disk: "<<endl;
	for(string s: d->files){
		cout<<s<<endl;
	}
			
}
void list_of_open_files(){

	Disk *d = disk[mounted_disk];
	if(d->fd_to_file_mode.empty()){
		cout<<"No open files are present"<<endl;
		return ;
	}
	cout<<"list of open files with file descriptor and mode: "<<endl;
	for(auto it= d->fd_to_file_mode.begin();it!= d->fd_to_file_mode.end();++it){
		cout<<it->second.first<<"\t"<<it->first<<"\t"<<it->second.second<<endl;
	}
			
}
/************************* MAIN FUNCTION **********************************/
int main(){
	int choice;
	while(1){
		cout<<"1. create disk"<<endl;
		cout<<"2. mount disk"<<endl;
		cout<<"3. exit"<<endl;
		cout<<"Select your Disk Menu: ";
		cin>>choice;
		if(choice == 1){
			cout<<"Enter a disk name: ";
			char arr[100];
			cin>>arr;
			cout<<create_disk(arr)<<endl;
		}
		else if(choice == 2){
			cout<<"Enter a created disk name to mount: ";
			char arr[100];
			cin>>arr;
			if(mount_disk(arr)){
				cout<<arr<<" disk is mounted"<<endl;
			}
			else{
				cout<<arr<<" disk does not exist."<<endl;
				continue;
			}
			/* section2 menu */
			int sel,fd;
			while(1){
				cout<<"\n1. create file"<<endl;
				cout<<"2. open file"<<endl;
				cout<<"3. read file"<<endl;
				cout<<"4. write file"<<endl;
				cout<<"5. append file"<<endl;
				cout<<"6. close file"<<endl;
				cout<<"7. delete file"<<endl;
				cout<<"8. list of files"<<endl;
				cout<<"9. list pf open files"<<endl;
				cout<<"10. unmount"<<endl;
				cout<<"\nEnter your choice: ";
				cin>>sel;
				char fn[100];
				char mode[10];
				string str;
				int option;
				if(sel == 10){
					
					unmount_disk();
					cout<<"Unmounted Successfully."<<endl;
					break;	
				}
				string f_name="";
				switch(sel){
					case 1:
						memset(fn,0,100*sizeof(char));
						cout<<"Enter a file name: ";
						cin>>fn;
						create_file(fn);
						break;
					case 2:
						memset(fn,0,100*sizeof(char));
						cout<<"Enter a file name: ";
						cin>>fn;
						cout<<"\n0. read mode"<<endl;
						cout<<"1. write mode"<<endl;
						cout<<"2. append mode"<<endl;
						cout<<"Select a option: ";
						cin>>option;
						cout<<endl;
						str ="";
						if(option == 0){
							str = "r";
							strcpy(mode,str.c_str());
							cout<<"file descriptor = "<<open_file(fn,mode)<<endl;
						}
						else if(option == 1){
							str = "w";
							strcpy(mode,str.c_str());
							cout<<"file descriptor = "<<open_file(fn,mode)<<endl;
						}
						else if(option == 2){
							str = "a";
							strcpy(mode,str.c_str());
							cout<<"file descriptor = "<<open_file(fn,mode)<<endl;
						}
						else{
							cout<<"Invalid mode "<<endl;
							break;
						}
						
						break;
					case 3:
						cout<<"Enter the file descriptor: ";
						cin>>fd;
						read_file(fd);
						break;
					case 4:
						cout<<"Enter the file descriptor: ";
						cin>>fd;
						write_file(fd);
						break;
					
					case 5:
						cout<<"Enter the file descriptor: ";
						cin>>fd;
						write_file(fd);
						break;
					case 6:
						cout<<"Enter the file descriptor: ";
						cin>>fd;
						close_file(fd);
						break;
					case 7:
						//cout<<"Enter the file descriptor: ";
						//cin>>fd;
						
						cout<<"Enter the file name: ";
						cin>>f_name;
						//delete_file(fd);
						delete_file(f_name);
						break;
					case 8:

						list_of_files();
						break;	
					case 9:
						list_of_open_files();
						break;
					
					default:
						cout<<"\nInvalid choice."<<endl;
						
				}
			}
			
		}
		else if(choice == 3){
			
			break;
		}
		else{
			cout<<"Please select a invalid choice."<<endl;
		}
		
	}
	return 0;
}
