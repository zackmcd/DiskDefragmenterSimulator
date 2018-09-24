#include "defragmenter.h"
#include "DefragRunner.h"

Defragmenter::Defragmenter(DiskDrive *diskDrive)
{
  int EMPTY = 0;
  int find = diskDrive->getCapacity() - 1;
  for (; find >= 2; find--)
  {
    if (!diskDrive->FAT[find])
      break;
  }
  EMPTY = find;

  int numArray[diskDrive->getCapacity()];
  for (int i = 0; i < diskDrive->getCapacity(); i++)
    numArray[i] = 0;

  int endArray[diskDrive->getCapacity()];
  for (int i = 0; i < diskDrive->getCapacity(); i++)
    endArray[i] = 0;

  DiskBlock *buffer1 = diskDrive->readDiskBlock(0); // to stop it from seg faulting
  DiskBlock *buffer2 = diskDrive->readDiskBlock(0); // to stop it from seg faulting
  DiskBlock *buffer = diskDrive->readDiskBlock(0); // to stop it from seg faulting
  DiskBlock *null = diskDrive->readDiskBlock(1);
  null = buffer; // for warning

  int COUNT = 0;

  DiskBlock **tempArray;
  tempArray = new DiskBlock* [diskDrive->getCapacity()];
  
  int correctPos = 2;
  int fileNum = 0;
  for (; fileNum < diskDrive->getNumFiles(); fileNum++)
  {
    int currentPos = diskDrive->directory[fileNum].getFirstBlockID();
    diskDrive->directory[fileNum].setFirstBlockID(correctPos); // changes the first block id to its new spot *************
    do
    {
      if (currentPos != correctPos && numArray[currentPos] != correctPos)
      {
        if (COUNT == 7500 && diskDrive->FAT[correctPos] == true)
        {
          DiskBlock *movingIn = null;
          bool found = true;
           
          int openSpot = EMPTY;     

          if (numArray[currentPos] != 0)
          {
            movingIn = diskDrive->readDiskBlock(numArray[currentPos]);
            diskDrive->FAT[numArray[currentPos]] = false;   
            EMPTY = numArray[currentPos];
            endArray[numArray[currentPos]] = 0; // &&&&
            numArray[currentPos] = 0;
            found = false;
          }
          else
            EMPTY = currentPos; //&&&&

          if (tempArray[currentPos] != NULL)
          {
            movingIn = tempArray[currentPos];
            tempArray[currentPos] = NULL;
            COUNT--;
            found = false; //**********
          }

          if (movingIn->getNext() == 0)
            movingIn = diskDrive->readDiskBlock(currentPos);

          DiskBlock *movingOut = diskDrive->readDiskBlock(correctPos);
          diskDrive->writeDiskBlock(movingIn, correctPos);

          diskDrive->writeDiskBlock(movingOut, openSpot);
          diskDrive->FAT[openSpot] = true;
          

          if (found)
            diskDrive->FAT[currentPos] = false;

          bool checker = false;
          int i = 0;
          i = endArray[correctPos];
          if (i > 0)
            checker = true;

          if (checker)
          {
            numArray[i] = openSpot;
            endArray[correctPos] = 0; // &&&&&
            endArray[openSpot] = i; // &&&&
          }
          else
          {
            numArray[correctPos] = openSpot;
            endArray[openSpot] = correctPos; //&&&&
          } 

          currentPos = movingIn->getNext();
          if (movingIn->getNext() != 1)
            movingIn->setNext(correctPos + 1);
          correctPos++;
          
          delete movingIn;
          delete movingOut;
        }
        else
        {
          bool checker2 = false;
          int i = 0;
          i = endArray[correctPos];//&&&&
          if (i > 0)//&&&&
            checker2 = true;//&&&&

          if (checker2)
          {
            DiskBlock *movingOut2 = diskDrive->readDiskBlock(correctPos);
            int openSpot2 = 0;
            for (openSpot2 = diskDrive->getCapacity() - 1; openSpot2 >= 2; openSpot2--)
            {  
              if (!diskDrive->FAT[openSpot2])
                break;
            }

            diskDrive->writeDiskBlock(movingOut2, openSpot2);
            diskDrive->FAT[openSpot2] = true;
            numArray[i] = openSpot2;
            endArray[correctPos] = 0; // &&&&
            endArray[openSpot2] = i; // &&&&
            delete movingOut2;
          }

          DiskBlock *ramFull = null;
          if (numArray[currentPos] != 0)
          {
            int difValue = numArray[currentPos];
            ramFull = diskDrive->readDiskBlock(difValue);
            diskDrive->FAT[difValue] = false;
            EMPTY = difValue; // &&&
            endArray[numArray[currentPos]] = 0; // &&&&
            numArray[currentPos] = 0; // correctPos
          }
          else
            EMPTY = currentPos; // &&&&&

          DiskBlock *check = null;
          if (ramFull->getNext() != 0)
            check = ramFull;
          else
            check = tempArray[correctPos];
          if (tempArray[correctPos] == NULL)
          {
            if (diskDrive->FAT[correctPos] == true)
            {
              tempArray[correctPos] = diskDrive->readDiskBlock(correctPos);
              COUNT++;
            }

            if (tempArray[currentPos]) 
            {
              DiskBlock *temp1 = tempArray[currentPos];
              diskDrive->writeDiskBlock(temp1, correctPos);
              tempArray[currentPos] = NULL;
              COUNT--;
              diskDrive->FAT[correctPos] = true; // ******
              delete temp1;
            }
            else
            {
              DiskBlock *temp2;
              if (check)
                temp2 = check;
              else
                temp2 = diskDrive->readDiskBlock(currentPos);

              diskDrive->writeDiskBlock(temp2, correctPos);
 
              delete temp2;

              if (diskDrive->FAT[correctPos] == false)
                diskDrive->FAT[correctPos] = true;
              if (!check)
                diskDrive->FAT[currentPos] = false;
            }
          } 
          else
          {
            if (diskDrive->FAT[correctPos] == true) 
            {
              tempArray[correctPos] = diskDrive->readDiskBlock(correctPos);
              COUNT++;
            }

            DiskBlock *temp3;
            if (check)
              temp3 = check;
            else
              temp3 = tempArray[currentPos];
            diskDrive->writeDiskBlock(temp3, correctPos);
            delete temp3;
            tempArray[currentPos] = NULL;
            COUNT--;
            diskDrive->FAT[correctPos] = true; //*****
          }
          DiskBlock *temp4 = diskDrive->readDiskBlock(correctPos);
          currentPos = temp4->getNext();
          delete temp4;
          correctPos++;
        }
      }
      else
      {
        DiskBlock *temp5;

        if (numArray[currentPos] == correctPos)
        {
          temp5 = diskDrive->readDiskBlock(numArray[currentPos]);
          endArray[numArray[currentPos]] = 0; // &&&&
          numArray[currentPos] = 0;
        }
        else
          temp5 = diskDrive->readDiskBlock(currentPos);

        currentPos = temp5->getNext();
        if (temp5->getNext() != 1)
          temp5->setNext(correctPos + 1);
        correctPos++;
        delete temp5;
      }
    } while (currentPos != 1 || currentPos == 0);
  } // to iterate through the directory and getting the firstBlockID for each file   

  delete buffer1; // to stop it from seg faulting
  delete buffer2; // to stop it from seg faulting
} // Defragmenter()
