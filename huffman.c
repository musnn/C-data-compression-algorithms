
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
struct MinHeapNode{ // A Huffman tree node
  char cData;
  int nFreq;
  struct MinHeapNode *pLeft, *pRight;
};

struct MinHeap{ // min-heap (or Huffman tree) nodes
  int nSize, nCapacity;
  struct MinHeapNode** ppArray;
};
struct EncodingTable{
  char cCharacter;
  char* encoding;
};
 
// allocate a new min heap node with given character and frequency
struct MinHeapNode* newNode(char cCharacter,int nFreq){
  struct MinHeapNode* pTemp=(struct MinHeapNode*)malloc(sizeof(struct MinHeapNode));
  pTemp->pLeft=pTemp->pRight = NULL;
  pTemp->cData=cCharacter;
  pTemp->nFreq=nFreq;
  return pTemp;
}
struct MinHeap* createMinHeap(int nCapacity){
  struct MinHeap* pHeap=(struct MinHeap*)malloc(sizeof(struct MinHeap));
  pHeap->nSize=0;
  pHeap->nCapacity=nCapacity;
  pHeap->ppArray=(struct MinHeapNode**)malloc(pHeap->nCapacity*sizeof(struct MinHeapNode));
  return pHeap;
}
 
void minHeapify(struct MinHeap* minHeap,int idx){ // swap two min heap nodes
  int nSmallest=idx;
  int nLeft=2*idx+1;
  int nRight=2*idx+2;
  // If left child is smaller than root, then make it smallest
  if(nLeft<minHeap->nSize && minHeap->ppArray[nLeft]->nFreq<minHeap->ppArray[nSmallest]->nFreq) nSmallest=nLeft;
  if(nRight<minHeap->nSize && minHeap->ppArray[nRight]->nFreq<minHeap->ppArray[nSmallest]->nFreq) nSmallest=nRight;
  if(nSmallest!=idx){ // Swap nodes
    struct MinHeapNode* temp=minHeap->ppArray[idx];
    minHeap->ppArray[idx]=minHeap->ppArray[nSmallest];
    minHeap->ppArray[nSmallest]=temp;

    minHeapify(minHeap,nSmallest); // Recursively heapify the affected sub-tree
  }
}

// store the Huffman code for a character in the encoding table
void storeEncoding(struct EncodingTable* pTable,char cCharacter,int arr[],int iTop){
  int index=0;
  while(pTable[index].cCharacter!='\0'){
      if(pTable[index].cCharacter==cCharacter) break;
      index++;
  }
  pTable[index].cCharacter=cCharacter;
  pTable[index].encoding=(char*)malloc((iTop+1)*sizeof(char));
  for(int i=0;i<iTop;i++) pTable[index].encoding[i]=arr[i]+'0';
  pTable[index].encoding[iTop]='\0';
}

struct MinHeapNode* extractMin(struct MinHeap* pMinHeap){ // extract minimum value node from heap
  struct MinHeapNode* temp=pMinHeap->ppArray[0];
  pMinHeap->ppArray[0]=pMinHeap->ppArray[pMinHeap->nSize-1];
  --pMinHeap->nSize;
  minHeapify(pMinHeap,0);
  return temp;
}
 
// insert a new node to Min Heap
void insertMinHeap(struct MinHeap* pMinHeap,struct MinHeapNode* pNode){
  pMinHeap->nSize++;
  int index=pMinHeap->nSize-1;
  while(index>0){
    int iParent=(index-1)/2;
      struct MinHeapNode* pParentNode=pMinHeap->ppArray[iParent];
      if(pNode->nFreq>=pParentNode->nFreq) break;
      pMinHeap->ppArray[index]=pParentNode;
      index=iParent;
  }
  pMinHeap->ppArray[index]=pNode;
}

 
// Creates a min heap of capacity equal to size and inserts all character of cData[] in min heap
struct MinHeap* createAndBuildMinHeap(char cData[],int nFreq[],int nSize){
  struct MinHeap* pMinHeap=createMinHeap(nSize);
  for (int i=0;i<nSize;i++){
    struct MinHeapNode* pNode=newNode(cData[i],nFreq[i]);
    insertMinHeap(pMinHeap,pNode);
  }
  return pMinHeap;
}
 
// build Huffman tree
struct MinHeapNode* buildHuffmanTree(char cData[],int nFreq[],int nSize){
  struct MinHeapNode *pLeft,*pRight,*pTop;
  struct MinHeap* pMinHeap=createMinHeap(nSize);  //Create a min heap of capacity equal to size 
  for (int i=0;i<nSize;i++){
    struct MinHeapNode* node=newNode(cData[i],nFreq[i]);
    insertMinHeap(pMinHeap,node);
  }
  while(!(pMinHeap->nSize==1)){
    // Extract the two minimum freq items from min heap
    pLeft=extractMin(pMinHeap);
    pRight=extractMin(pMinHeap);
    /* Create a new node with frequency equal to the sum of the two nodes frequencies.
    Make the two extracted node as left and right children of this new node.
    Add this node to the min heap */
    pTop=newNode('$',pLeft->nFreq+pRight->nFreq);
    pTop->pLeft=pLeft;
    pTop->pRight=pRight;
    insertMinHeap(pMinHeap,pTop);
  }
  return extractMin(pMinHeap); //The remaining node is root node and the tree is complete
}
 
// traverse the Huffman tree to generate the encoding for each leaf node. updates the encoding table by calling the storeEncoding function.
void traverseHuffmanTree(struct MinHeapNode* pRoot,int arr[],int iTop,struct EncodingTable* pTable){
  if(pRoot==NULL) return;
  if(!(pRoot->pLeft) && !(pRoot->pRight)) storeEncoding(pTable,pRoot->cData,arr,iTop);
  arr[iTop]=0;
  traverseHuffmanTree(pRoot->pLeft,arr,iTop+1,pTable);
  arr[iTop]=1;
  traverseHuffmanTree(pRoot->pRight,arr,iTop+1,pTable);
}
 
char* decodeText(const char* cEncodedText,struct MinHeapNode* pRoot){
  int nBuffer=256;
  char* cDecodedText=(char*)malloc(nBuffer*sizeof(char));
  int iDecoded=0;
  struct MinHeapNode* pCurrent=pRoot;
  for(int i=0;cEncodedText[i]!='\0';i++) {
    // if the bit is 0, move to the left subtree, else move to the right subtree
    pCurrent=(cEncodedText[i]=='0')?pCurrent->pLeft:pCurrent->pRight;
    if(!pCurrent->pLeft && !pCurrent->pRight){ // if it's a leaf node, add the character to the decoded text
      if(iDecoded>=nBuffer-1){ // if the decoded text exceeds the buffer size, reallocate memory
        nBuffer*=2;
        cDecodedText=(char*)realloc(cDecodedText,nBuffer*sizeof(char));
      }
      cDecodedText[iDecoded++]=pCurrent->cData;
      pCurrent=pRoot;
    }
  }
  cDecodedText[iDecoded]='\0';
  return cDecodedText;
}

// builds the Huffman tree and generates the encoding for each leaf node with traversing
void HuffmanCodes(char cData[],int nFreq[],int nSize,struct EncodingTable* pTable){
  struct MinHeapNode* pRoot=buildHuffmanTree(cData,nFreq,nSize);
  int arr[100],iTop=0;
  traverseHuffmanTree(pRoot,arr,iTop,pTable);
}

char* getElements(const char* cSentence){
  char* cElements=(char*)malloc(256*sizeof(char));
  memset(cElements,0,256);
  // Loop through sentence and mark each character as present
  for (int i=0;cSentence[i]!='\0';i++) cElements[(int)cSentence[i]]=1;
  int nCount=0;
  for (int i=0;i<256;i++) if(cElements[i]==1) cElements[nCount++]=(char)i; // Copy marked characters into new array
  cElements[nCount]='\0';
  return cElements;
}
// encoding of the text with the help of the table
char* encodeText(const char* text,struct EncodingTable* pTable){
  int nBuffer=256;
  char* cEncodedText=(char*)malloc(nBuffer*sizeof(char));
  int cEncodedTextIndex=0;
  for(int i=0;text[i]!='\0';i++){ // untill the end of text[i]
    int tableIndex=0;
    while(pTable[tableIndex].cCharacter!='\0'){
      if(pTable[tableIndex].cCharacter==text[i]) break;  // If the character is found in encoding table exit
      tableIndex++;
    }
    const char* encoding=pTable[tableIndex].encoding;
    int encodingLength=strlen(encoding);  
    if(cEncodedTextIndex+encodingLength>=nBuffer-1){ // Check if buffer needs to be resized
      nBuffer*=2;
      cEncodedText=(char*)realloc(cEncodedText,nBuffer*sizeof(char));
    }
    strcpy(cEncodedText+cEncodedTextIndex,encoding); // Copy the binary encoding to the buffer
    cEncodedTextIndex+=encodingLength;
  }
  cEncodedText[cEncodedTextIndex] = '\0';

  return cEncodedText;
}

// Return an array of frequencies for each unique element in sentence
int* getElementFrequencies(const char* cSentence){
  int* nFreq=(int*)calloc(256,sizeof(int));
  for (int i=0;cSentence[i]!='\0';i++) nFreq[(int)cSentence[i]]++;   // Loop through sentence and count each character's frequency
  char* cElements=getElements(cSentence); // Create new array with frequencies of unique elements
  int nCount=strlen(cElements);
  int* nElementFreq=(int*)malloc(nCount*sizeof(int));
  for (int i=0;i<nCount;i++) nElementFreq[i]=nFreq[(int)cElements[i]];
  free(cElements);
  free(nFreq);
  return nElementFreq;
}

// Run 
int main(){
  char cSentence[]="eddie edited it did eddie edit it";

  char* cElements=getElements(cSentence);
  int* nFrequency=getElementFrequencies(cSentence);
  int nSize=strlen(cElements);
  struct EncodingTable pTable[256]={0};
  HuffmanCodes(cElements,nFrequency,nSize,pTable);
  for(int i=0;i<nSize;i++) printf("%c: %s\n",pTable[i].cCharacter,pTable[i].encoding); // print out the dictionary
  char* cEncodedText=encodeText(cSentence,pTable);
  printf("Encoded text: %s\n",cEncodedText);

  struct MinHeapNode* pRoot=buildHuffmanTree(cElements,nFrequency,nSize);
  char* toDecode="0110100001111011000110011011100110111100010111011010000111101100011011100110";
  char* cDecodedText=decodeText(toDecode,pRoot);
  printf("Decoded text: %s\n",cDecodedText);

  free(cDecodedText);
  free(cEncodedText);
  free(cElements);
  free(nFrequency);

  return 0;
}