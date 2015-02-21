#include "storage_mgr.h"

#include <stdio.h> 
#include <sys/types.h> 
#include <sys/stat.h> 
#include <unistd.h> 
#include <errno.h> 
#include <string.h> 
#include <fcntl.h> 

#define FILE_NAME "test_pagefile.bin"


/* manipulating page files */
void initStorageManager (void){ 
printf("Hii\n");

}


/**METHOD:-createPageFile()
  Use-Case:-Create a File with one Page of 4096 null charecters(bytes) in it .
  Arguments:-fileName = The name of the file to be created.
  Return-Value:- To catch the possible exceptions.
**/
 RC createPageFile (char *fileName){ 
	printf("---------In createPageFile() method :-%s----------- \n",fileName);
	printf("Page Size is :-%d \n",PAGE_SIZE);

	char *buffer[PAGE_SIZE];int i = 0;int bytesWritten;
	FILE *fp;

	fp = fopen(fileName,"wb+");
	if(fp == NULL)
	    {
		return RC_FILE_NOT_FOUND;
		printf("Could Not Find the file");
	    }

	memset(buffer,'\0',PAGE_SIZE);
	printf("Inn");
	bytesWritten = fwrite(buffer ,1 , PAGE_SIZE,fp);
	
	if(bytesWritten < PAGE_SIZE) 
	      {
		printf("Write Failed :- No. of bytes written are %d\n",bytesWritten);
		return RC_WRITE_FAILED;
	      }
	else return RC_OK;
}

/**METHOD:-openPageFile()
  Use-Case:-Open the existing page and initialise all the variables in structure SM_FileHandle with details of the page.
  Arguments:-fileName = The name of the file created , fHandle = File Handler which handles information of the file.
  Return-Value:- To catch the possible exceptions.
**/
 RC openPageFile (char *fileName, SM_FileHandle *fHandle){ 
	printf ("----------------IN openPageFile methd------------------%s----\n",fileName);

	/*Open an existing page file.*/
	FILE *fpr;
	fpr = fopen(fileName , "rb+");
	if(fpr == NULL){
		return RC_FILE_NOT_FOUND;
	}
	//Initialze the fields in SM_FileHandle
	fHandle->fileName = fileName;
	fHandle->curPagePos = 0;
	

	int status = fseek(fpr,0,SEEK_END); 
	   if(status == -1){
		printf("Seek Failed");
		return RC_SEEK_FAILED;
	    }

	fHandle->totalNumPages = (ftell(fpr) / PAGE_SIZE );
	
	printf("seekdend is %lu--",ftell(fpr));	
	printf("Current Position is %d , File Name is %s , Total pages are %d\n",fHandle->curPagePos,fHandle->fileName,fHandle->totalNumPages);
	fHandle->mgmtInfo = ftell(fpr);
	return RC_OK;
}

/**METHOD:-closePageFile()
  Use-Case:-Close the open Page file.
  Arguments:-fileName = The name of the file to be closed.
  Return-Value:- To catch the possible exceptions.
**/
 RC closePageFile (SM_FileHandle *fHandle){
	printf("------------IN closePageFile ----------------%s---------------\n",fHandle->fileName);

	FILE *fpd = fopen(fHandle->fileName,"r");
	fseek(fpd,0,SEEK_SET);
	printf("Current Position is %d , File Name is %s , Total pages are %d\n",fHandle->curPagePos,fHandle->fileName,fHandle->totalNumPages);
	fclose(fpd);
	fpd = NULL; // Reset File Pointer to ensure file is closed properly

	/* Clean up the memory to prevent "memory leaks" */
	/*free(s_FH.fileName);*/

	return 0;

}

/**METHOD:-destroyPageFile()
  Use-Case:-Delete the opened Page File.
  Arguments:-fileName = The name of the file to be deleted.
  Return-Value:- To catch the possible exceptions.
**/
 RC destroyPageFile (char *fileName){ 
	printf("------------IN destroyPageFile ------------------\n");

	int ret = remove(fileName);
	if(ret == 0) 
   	{
      	printf("File deleted successfully");
	return RC_OK;
   	}
   	else 
   	{
      	printf("Error: unable to delete the file");
	return RC_DELETE_FAILED;
   	}

}

/**METHOD:-getBlockPos()
  Use-Case:-Used to get the Block position of the file where the cursor is pointed in based on the cursor position.
  Arguments:-fileName = The name of the file to be created.
  Return-Value:- The Block Position.
**/
int getBlockPos (SM_FileHandle *fHandle){ 
	
	int positionInBytes = fHandle->mgmtInfo;
	printf("--xxx----yyy---%d----\n",positionInBytes);

	int blockNumber = positionInBytes / PAGE_SIZE ;

	return blockNumber;
	
 }
/*METHOD:-readFirstBlock()
  Use-Case:-Read first block from 0 to 4095 Bytes to Page Handle 
  Arguments:-SM_FileHandle = Contains all the details relating to the File , memPage = Page Handler which reads the data 
  Return-Value:- To catch the possible exceptions. 
1. Read first block from 0 to 4096 Byte to Page Handle */
 RC readFirstBlock (SM_FileHandle *fHandle, SM_PageHandle memPage){ 
	printf("-----------In readFirstBlock Method------%s------------\n",fHandle->fileName);
	
	FILE *fpRFB = fopen(fHandle->fileName,"rb+");
	  if(fpRFB == NULL){
		printf("Not Found!!");
		return RC_FILE_NOT_FOUND;
	   }
	int seekStatus = fseek(fpRFB, 0, SEEK_SET);
	//rewind (fpRFB);
	   if(seekStatus == -1){
		printf("Seek Failed");
		return RC_SEEK_FAILED;
	    }
	printf("0000--%lu---",ftell(fpRFB));
	fHandle->mgmtInfo = ftell(fpRFB);
	
	//printf("Before FREE():-%d",memPage[4000]);
	free(memPage);
	//memPage = NULL;
	//printf("After FREE():-%d",memPage[4000]);

	int status = readBlocks(fpRFB,fHandle,memPage);
	printf("status is %d\n",status);
	//memPage = (SM_PageHandle) malloc(4096);
	//int x = fread(memPage, 1,4096, fpRFB);
	//printf("Testing Valuee :- %d \n",x);
	//printf("Size of memPage(1) is %d \n",memPage[1]);
	return status;
 }
/* 2. Read block of 4096 Bytes pecified by Page number */
/**METHOD:-readBlock()
  Use-Case:-Read a block based on the page number of 4096 Bytes to Page Handle 
  Arguments:-pageNum = The page number of the file to be read , SM_FileHandle = Contains all the details relating to the File , 
             memPage = Page Handler which reads the data.
  Return-Value:- To catch the possible exceptions.
**/
 RC readBlock (int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage){ 
	printf("-----------In readBlock Method------%s------------\n",fHandle->fileName);
	
	FILE *fpRB = fopen(fHandle->fileName,"rb");
	   if(fpRB == NULL){
		printf("Not Found!!");
		return RC_FILE_NOT_FOUND;
	    }
	int seekStatus = fseek(fpRB, pageNum*PAGE_SIZE, SEEK_SET);
	   if(seekStatus == -1){
		printf("Seek Failed");
		return RC_SEEK_FAILED;
	    }
	fHandle->mgmtInfo = ftell(fpRB);
	
	int status = readBlocks(fpRB,fHandle,memPage);
	printf("status is %d\n",status);
	
	//fseek(fpRB, PAGE_SIZE*pageNum , SEEK_SET);
	//long fsize = ftell(fpRB);
	//printf("Moved to %lu Bytes\n",fsize);
	//fseek(fpRB, 0, SEEK_SET);

	//memPage = malloc(fsize);
	//fread(memPage, fsize, 1, fpRB);
	//fHandle->mgmtInfo = ftell(fpRB);
	//printf("Testing Valuee :- %c \n",memPage[3]);

	return status;
 }

/*3. Read Previous Block based on the current cursor position*/
/**METHOD:-readPreviousBlock()
  Use-Case:-Read Previous Block based on the current cursor position.
  Arguments:-SM_FileHandle = Contains all the details relating to the File , memPage = Page Handler which reads the data 
  Return-Value:- To catch the possible exceptions.
**/
 RC readPreviousBlock (SM_FileHandle *fHandle, SM_PageHandle memPage){ 
	printf("---------In Read Previous Block Method ---- Arguments are:-%s---with position :-%d ---\n",fHandle->fileName,fHandle->mgmtInfo);

	int currentCursorPosition = fHandle->mgmtInfo;
	int currentBlock = (currentCursorPosition / PAGE_SIZE );  //300(0),4096(0),8012(1),5000(0)
	int previousBlock; 
	if(currentBlock == 0)
	previousBlock = 0;
	else
	previousBlock = currentBlock - 1 ;

	printf("Moving to Previous Block %d\n",previousBlock);

	FILE *fpRPB = fopen(fHandle->fileName,"rb");
	   if(fpRPB == NULL){
		printf("Not Found!!");
		return RC_FILE_NOT_FOUND;
	    }
	int seekStatus = fseek(fpRPB, PAGE_SIZE*previousBlock , SEEK_SET);
	    if(seekStatus == -1){
		printf("Seek Failed");
		return RC_SEEK_FAILED;
	    }
	fHandle->mgmtInfo = ftell(fpRPB);
	int status = readBlocks(fpRPB,fHandle,memPage);
	printf("status is %d\n",status);

	//memPage = malloc(PAGE_SIZE);
	//fread(memPage, PAGE_SIZE, 1, fpRPB);
	//printf("Testing Valuee :- %c \n",memPage[3]);
	
	//fHandle->mgmtInfo = ftell(fpRPB);
	//printf("Present cursor Position after reading previous block is %lu\n",ftell(fpRPB));	

	return status;
 
 }

/*4. Read Current Block based on the current cursor position*/
/**METHOD:-readCurrentBlock()
  Use-Case:-Read Current Block based on the current cursor position
  Arguments:-SM_FileHandle = Contains all the details relating to the File , memPage = Page Handler which reads the data 
  Return-Value:- To catch the possible exceptions.
**/
 RC readCurrentBlock (SM_FileHandle *fHandle, SM_PageHandle memPage){ 
	printf("------In Read Current Block Method : Arguments are:---%s---with position:-%d---\n",fHandle->fileName,fHandle->mgmtInfo);

	int currentBlock ;
	int currentCursorPosition = fHandle->mgmtInfo;    	 // 300(0),4096(1),8012(2),5000(1)

	currentBlock = (currentCursorPosition / PAGE_SIZE ) ; 
	
	printf("Current Block is %d\n",currentBlock);

	FILE *fpRCB = fopen(fHandle->fileName,"rb");
	    if(fpRCB == NULL){
		printf("Not Found!!");
		return RC_FILE_NOT_FOUND;
	    }
	int seekStatus = fseek(fpRCB, PAGE_SIZE*currentBlock , SEEK_SET);
	    if(seekStatus == -1){
		printf("Seek Failed");
		return RC_SEEK_FAILED;
	    }
	//long fsize = ftell(fpRCB);
	//printf("Moved to %lu Bytes\n",fsize);

	fHandle->mgmtInfo = ftell(fpRCB);
	int status = readBlocks(fpRCB,fHandle,memPage);
	printf("status is %d\n",status);

	//memPage = malloc(fsize);
	//fread(memPage, PAGE_SIZE, 1, fpRCB);
	//printf("Testing Valuee :- %c \n",memPage[3]);

	//fHandle->mgmtInfo = ftell(fpRCB);
	//printf("Present cursor Position after reading current block is %lu\n",ftell(fpRCB));	

	return status;

 }

/*5. Read Next Block based on the current cursor position*/
/**METHOD:-readNextBlock()
  Use-Case:-Read Next Block based on the current cursor position.
  Arguments:-SM_FileHandle = Contains all the details relating to the File , memPage = Page Handler which reads the data 
  Return-Value:- To catch the possible exceptions.
**/
 RC readNextBlock (SM_FileHandle *fHandle, SM_PageHandle memPage){ 
	printf("------In Read Next Block Method : Arguments are:---%s---with position:-%d---\n",fHandle->fileName,fHandle->mgmtInfo);

	int nextBlock ;
	int currentCursorPosition = fHandle->mgmtInfo;    	 // 300(1),4096(2),8012(3),5000(2)
	
	nextBlock = (currentCursorPosition / PAGE_SIZE ) + 1 ;

	printf("Next Block is %d\n",nextBlock);
	
	FILE *fpRNB = fopen(fHandle->fileName,"wb");
	    if(fpRNB == NULL){
		printf("Not Found!!");
		return RC_FILE_NOT_FOUND;
	    }
	int seekStatus = fseek(fpRNB, PAGE_SIZE*nextBlock , SEEK_SET);
	    if(seekStatus == -1){
		printf("Seek Failed");
		return RC_SEEK_FAILED;
	    }
	fHandle->mgmtInfo = ftell(fpRNB);
	int status = readBlocks(fpRNB,fHandle,memPage);
	printf("status is %d\n",status);

	//	memPage = malloc(fsize);
	//	fread(memPage, PAGE_SIZE, 1, fpRNB);
	//	printf("Testing Valuee :- %c \n",memPage[3]);

	//	fHandle->mgmtInfo = ftell(fpRNB);
	//	printf("Present cursor Position after reading current block is %lu\n",ftell(fpRNB));	

		return status;
	
 }
/*6. Read Last Block based on the current cursor position*/
/**METHOD:-readLastBlock()
  Use-Case:-Read Last Block based on the current cursor position.
  Arguments:-SM_FileHandle = Contains all the details relating to the File , memPage = Page Handler which reads the data 
  Return-Value:- To catch the possible exceptions.
**/
 RC readLastBlock (SM_FileHandle *fHandle, SM_PageHandle memPage){ 
	printf("-----------In readLastBlock Method------%s------------\n",fHandle->fileName);
	
	FILE *fpRFB = fopen(fHandle->fileName,"rb");
	   if(fpRFB == NULL){
		printf("Not Found!!");
		return RC_FILE_NOT_FOUND;
	    }
	int seekStatus = fseek(fpRFB, -(PAGE_SIZE-1) , SEEK_END);
           if(seekStatus == -1){
		printf("Seek Failed");
		return RC_SEEK_FAILED;
	    }
	fHandle->mgmtInfo = ftell(fpRFB);
	printf("--%lu---",ftell(fpRFB));
	int status = readBlocks(fpRFB,fHandle,memPage);
	printf("status is %d\n",status);

	//memPage = malloc(fsize);
	//fread(memPage, fsize, 1, fpRFB);
	//printf("Testing Valuee :- %c \n",memPage[3]);

	return status;

 }
 

/*7.Write the block back based on pagenumber*/
/**METHOD:-writeBlock()
  Use-Case:-Write the block back based on pagenumber in the page handler to the file Handler
  Arguments:-pageNum = The page number of the file to be written , SM_FileHandle = Contains all the details relating to the File , 
             memPage = Page Handler which writes the data.
  Return-Value:- To catch the possible exceptions.
**/
 RC writeBlock (int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage){ 

	int bytesWritten;
	printf("-----------In writeBlock Method----Arguments are:---%s--and Page number is %d----------\n",fHandle->fileName,pageNum);
	FILE *fpWB = fopen(fHandle->fileName,"wb");
	    if(fpWB == NULL){
		printf("Not Found!!");
		return RC_FILE_NOT_FOUND;
	      }

	int TOTAL_BLOCKS =  getTotalNumberOfBlocks(fpWB);

	if(pageNum>TOTAL_BLOCKS)
	{ 
		printf("This is the last page");
		fseek(fpWB,0,SEEK_SET);
		fHandle->mgmtInfo = ftell(fpWB);
		return RC_READ_NON_EXISTING_PAGE;
	}

	int seekStatus = fseek(fpWB, PAGE_SIZE*pageNum , SEEK_SET);
	    if(seekStatus == -1){
		printf("Seek Failed");
		return RC_SEEK_FAILED;
	      }
	long fsize = ftell(fpWB);
	printf("Started to write from %lu Byte (Cursor-Position )\n",fsize);
	
	bytesWritten = fwrite(memPage,1,PAGE_SIZE,fpWB);
	fHandle->mgmtInfo = ftell(fpWB);
	printf("Present cursor Position after writing 4096 Bytes is %lu\n",ftell(fpWB));	

	if(bytesWritten < PAGE_SIZE) 
	      {
		printf("Write Failed :- No. of bytes written are %d\n",bytesWritten);
		return RC_WRITE_FAILED;
	      }
	else  return RC_OK;

}

/*8.Write Current Block to a page file */
/**METHOD:-writeCurrentBlock()
  Use-Case:-Write Current Block from a page handler to the file Handler .
  Arguments:-SM_FileHandle = Contains all the details relating to the File,memPage = Page Handler which writes the data.
  Return-Value:- To catch the possible exceptions.
**/
 RC writeCurrentBlock (SM_FileHandle *fHandle, SM_PageHandle memPage){ 
	printf("------In Write Current Block Method : Arguments are:---%s---with position:-%d---\n",fHandle->fileName,fHandle->mgmtInfo);
	
	int bytesWritten;
	int currentCursorPosition = fHandle->mgmtInfo;
	int currentBlock = (currentCursorPosition / PAGE_SIZE );
	
	printf("Current Block is %d\n",currentBlock);

	FILE *fpWCB = fopen(fHandle->fileName,"wb");
	   if(fpWCB == NULL){
		printf("Not Found!!");
		return RC_FILE_NOT_FOUND;
	     }
	int seekStatus = fseek(fpWCB, PAGE_SIZE*currentBlock , SEEK_SET);
	   if(seekStatus == -1){
		printf("Seek Failed");
		return RC_SEEK_FAILED;
	     }
	long fsize = ftell(fpWCB);
	printf("Started to write from %lu Byte (Cursor-Position )\n",fsize);

	bytesWritten = fwrite(memPage,PAGE_SIZE,1,fpWCB);
	fHandle->mgmtInfo = ftell(fpWCB);
	printf("Present cursor Position after writing 4096 Bytes is %lu\n",ftell(fpWCB));	

	if(bytesWritten < PAGE_SIZE) 
	      {
		printf("Write Failed :- No. of bytes written are %d\n",bytesWritten);
		return RC_WRITE_FAILED;
	      }
	else  return RC_OK;
	
 }
/*9.Append empty block to the end of the file obtained from the last cursor position.*/
/**METHOD:-appendEmptyBlock()
  Use-Case:-Append a block at the end of the file . Move the cursor to the last position.
  Arguments:-SM_FileHandle = Contains all the details relating to the File
  Return-Value:- To catch the possible exceptions.
**/
 RC appendEmptyBlock (SM_FileHandle *fHandle){ 
	printf("------------In Append Empty Block Method ----Arguments are :-%s----\n",fHandle->fileName);

	char *buffer[PAGE_SIZE];	
	memset(buffer,'\0',PAGE_SIZE);

	FILE *fpAEB = fopen(fHandle->fileName,"rb+");
	    if(fpAEB == NULL){
		printf("Not Found!!");
		return RC_FILE_NOT_FOUND;
	     }
	int seekStatus = fseek(fpAEB,1,SEEK_END);
            if(seekStatus == -1){
		printf("Seek Failed");
		return RC_SEEK_FAILED;
	     }
	printf("Present cursor Position before appending is %lu\n",ftell(fpAEB));	

		printf("Trying to append Empty Block");

	int bytesWritten =fwrite(buffer ,1, PAGE_SIZE ,fpAEB);
	if(bytesWritten < PAGE_SIZE) 
	      {
		printf("Write Failed :- No. of bytes written are %d\n",bytesWritten);
		return RC_WRITE_FAILED;
	      }
	else{
	fHandle->mgmtInfo = ftell(fpAEB);
	printf("Present cursor Position after writing 4096 Bytes is %lu\n",ftell(fpAEB));	

	return RC_OK;
	    }
 }

/*10. If the file has less than numberOfPages pages then increase the size to numberOfPages .*/
/**METHOD:-ensureCapacity()
  Use-Case:-If the file has less than numberOfPages pages then increase the size to numberOfPages .
  Arguments:-numberOfPages = Capacity of the page . If it is greater than the number of pages present , append the remaining blocks.
  Return-Value:- To catch the possible exceptions.
**/
 RC ensureCapacity (int numberOfPages, SM_FileHandle *fHandle){ 
	printf("-------------In Ensure Capacity Method--Arguments are:- %s---%d\n",fHandle->fileName,numberOfPages);
	
	FILE *fpAEB = fopen(fHandle->fileName,"rb+");
	    if(fpAEB == NULL){
		printf("Not Found!!");
		return RC_FILE_NOT_FOUND;
	    }
	int seekStatus = fseek(fpAEB,0,SEEK_END);
            if(seekStatus == -1){
		printf("Seek Failed");
		return RC_SEEK_FAILED;
	    }
	printf("Present cursor Position before appending is %lu\n",ftell(fpAEB));	

	int i , j,bytesWritten;

	int existingBlocks = 5;
	int ADD_BLOCKS = numberOfPages - existingBlocks;

	if(ADD_BLOCKS<=0)
	{
		printf("Block was of sufficient cappacity");
	}
	else
	{
		printf("Need to ensure capacity by adding %d Blocks",ADD_BLOCKS);
		for(i=1;i<=ADD_BLOCKS;i++)
		   {
			printf("Trying to append Block No. %d at Memory Location %u\n",i,i*PAGE_SIZE);
			int seekStatus = fseek(fpAEB, 1 ,SEEK_END);
			    if(seekStatus == -1){
				printf("Seek Failed");
				return RC_SEEK_FAILED;
			    }
			char *buffer[PAGE_SIZE];
			memset(buffer,'\0',PAGE_SIZE);
	
			bytesWritten = fwrite(buffer , 1,PAGE_SIZE ,fpAEB);
		   }
		int seekStatus1 = fseek(fpAEB,0,SEEK_END);
		   if(seekStatus1 == -1){
			printf("Seek Failed");
			return RC_SEEK_FAILED;
	    	    }
		fHandle->mgmtInfo = ftell(fpAEB);
		printf("Present cursor Position after adding %d Blocks is %lu\n",ADD_BLOCKS,ftell(fpAEB));	
		if(bytesWritten < PAGE_SIZE) 
	      	{
			printf("Write Failed :- No. of bytes written are %d\n",bytesWritten);
			return RC_WRITE_FAILED;
	      	}
		else return RC_OK;

	}

 }
 
/**METHOD:-readBlocks()
  Use-Case:-A common method for all the read operations.Moves the cursor to the specific position and read the block from 
	    the File Handler to Page Handler
  Arguments:-FILE = A File Pointer to catch the instance of the file used in the operation.
	     SM_FileHandle = Contains all the details relating to the File,memPage = Page Handler which writes the data.
  Return-Value:- To catch the possible exceptions.
**/
 RC readBlocks(FILE *fpRB, SM_FileHandle *fHandle, SM_PageHandle memPage){
	printf("-----------In readBlock-Common Method------%s------------\n",fHandle->fileName);
	
	int bytesRead;
	int currentCursorPosition = fHandle->mgmtInfo;
	int currentBlock = (currentCursorPosition / PAGE_SIZE );
	
	printf("Current Block is %d\n",currentBlock);

	int TOTAL_BLOCKS =  getTotalNumberOfBlocks(fpRB);

	if(currentBlock>TOTAL_BLOCKS)
	{ 
		printf("This is the last page");
		fseek(fpRB,0,SEEK_SET);
		fHandle->mgmtInfo = ftell(fpRB);
		return RC_READ_NON_EXISTING_PAGE;
	}
	else{
		
		int seekStatus = fseek(fpRB, PAGE_SIZE*currentBlock , SEEK_SET);
		   if(seekStatus == -1){
			printf("Seek Failed");
			return RC_SEEK_FAILED;
	    	    }
		long fsize = ftell(fpRB);
		printf("Moved toooo %lu Bytes\n",fsize);

		memPage = malloc(PAGE_SIZE);
		bytesRead = fread(memPage,1, PAGE_SIZE, fpRB);
		printf("Testing Valuee 1:- %c \n",memPage[1]);
		printf("Bytes Read:- %d\n",bytesRead);

		fHandle->mgmtInfo = ftell(fpRB);
		printf("Present cursor Position after reading current block is %lu\n",ftell(fpRB));	


		if(bytesRead < PAGE_SIZE) 
	      	    {
			printf("Read Failed :- No. of bytes read are %d\n",bytesRead);
			return RC_READ_FAILED;
	            }
		else  return RC_OK;
	}
 }

 int getTotalNumberOfBlocks(FILE *fpNOB){
	struct stat status;
    	fstat(fileno(fpNOB),&status);
    	printf("Size of file : %d",status.st_size);
	int FILE_SIZE = status.st_size;
	int numberOfBlocks;
	if(FILE_SIZE <= PAGE_SIZE){
	numberOfBlocks = 0;
	}
	else{
	numberOfBlocks = (FILE_SIZE / PAGE_SIZE) ;
	}

	return numberOfBlocks;
 }
