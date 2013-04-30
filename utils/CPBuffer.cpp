#include <sys/mman.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <CPBuffer.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <stdio.h>

CPBuffer::CPBuffer( long long int size, long long int cp, long long res, const char *name )
{
  mSize = alSize(size);
  mCP = alSize(cp);
  mRes = alSize(res);
  
  mValid = 0;
  int retry = 0;
  if( checkFile( name )>=0 )
    allocMem( name );
}

CPBuffer::~CPBuffer()
{
  if( mValid!=0 )
  {
    if( mpRes!=(void *)-1 ) 
      munmap( mpRes, (size_t)mRes );
    if( mpCP!=(void *)-1 ) 
      munmap( mpCP, (size_t)mCP );
    if( mpStart!=(void *)-1 )
      munmap( mpStart, (size_t)mSize );
  }
}

int CPBuffer::allocMem( const char *name )
{
  int error = -5;
  void *ptry;
  mpRes = mpCP = mpStart = (void *)-1;
  int fd = open(name,O_RDWR);
  if (fd<0)
  {
    error = -4;
    goto ERRF;
  }
  if( mRes!=0 )
  {
    mpRes = mmap(0,(size_t)mRes, PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
    if( mpRes==(void *)-1 )
    {
      error = -1;
      goto ERR;
    }
  }
  ptry = mmap(0,(size_t)(mSize+mCP), PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS,-1,0);
  munmap( ptry, (size_t)(mSize+mCP) );
  if( ptry!=(void *)-1 )
  {
    mpStart = mmap(ptry,(size_t)mSize, PROT_READ|PROT_WRITE,MAP_SHARED,fd,mRes);
    mpCP = mmap(ptry+(size_t)mSize, (size_t)mCP, PROT_READ|PROT_WRITE,MAP_SHARED|MAP_FIXED,fd,mRes);
  }
  else
  {
    error = -2;
    goto ERR;
  }
  if( mpStart == ptry && mpCP == ptry+(size_t)mSize )
  { 
    mValid = 1;
    strcpy( mName, name);
    close(fd);
    return 0;
  }
ERR:
  if( mpRes!=(void *)-1 ) 
    munmap( mpRes, (size_t)mRes );
  if( mpCP!=(void *)-1 ) 
    munmap( mpCP, (size_t)mCP );
  if( mpStart!=(void *)-1 )
    munmap( mpStart, (size_t)mSize );
  close(fd);
ERRF:
  mpRes = mpCP = mpStart = (void *)-1;
  mValid = 0;
  return error;
}

void *CPBuffer::getBuf( long long from, long long len )
{
  long long rFrom = from%mSize;
  if( rFrom+len > mSize+mCP || mValid==0 )
    return NULL;
  else
    return mpStart+(size_t)rFrom;
}

long long CPBuffer::getOff( void *buf )
{
  size_t off = buf-mpStart;
  long long ret = ( long long )off;
  if( ret<0 )
    return -1;
  else
    return ret;
}
int CPBuffer::valid()
{
  return mValid;
}

long long CPBuffer::alSize( long long sz )
{
  long long pageSize = (long long)getpagesize();
  return ((sz+pageSize-1)/pageSize)*pageSize;
}

int CPBuffer::checkFile( const char *name )
{
  struct stat st;
  int fd;
  int err = stat(name,&st);
  if( err ==-1 )
  {
    if( errno == ENOENT )
    {
      fd = open(name,O_RDWR|O_CREAT|O_EXCL,0666);
      if( fd>0 )
      {
        lseek(fd,mSize+mRes-1,SEEK_SET);
        write(fd,"0",1);
        close(fd);
        return 1;
      }
      if( errno == EEXIST )
      {
        return 0;
      }
    }
    return -1;
  }
  if( st.st_size == mSize+mRes )
    return 0;
  fd = open(name,O_RDWR|O_TRUNC);
  if( fd>0 )
  {
    lseek(fd,mSize+mRes-1,SEEK_SET);
    write(fd,"0",1);
    close(fd);
    return 1;
  }
  return -2;
}

void *CPBuffer::attach()
{
  if( mValid )
    return mpRes;
  else
    return NULL;
}  

long long CPBuffer::getSize()
{
  return mSize;
}