squaretolist = function(m) 
{ 
  lapply(
    apply(m,1,function(x) 
      {
        which(x==1) 
      }
    ), 
    function(x)
    { 
      if(length(x)) { x }
      else { NA }   
    }
  )
}

squaretothin = function(m)
{
  x=matrix(ncol=2)  
  for(i in 1:nrow(m))
  {
    ones = which(m[i,]==1)
    if(length(ones))
    {
      x = rbind(x,cbind(i,ones))
    }
  }
  colnames(x)=c("","")
  x[-1,]
}

thintolist = function(thin,nvert)
{
  squaretolist(thintosquare(thin,nvert)) 
}

listtothin = function(inlist)
{
  m = matrix(ncol = 2)
  for(i in 1:length(inlist))
  {
    if(!is.na(inlist[[i]]))
    {
      m = rbind(m, cbind(i,inlist[[i]]))
    }
  }
  colnames(m)=c("","")
  m[-1,]
}

listtosquare = function(inlist)
{
  thintosquare(listtothin(inlist),length(inlist))
}

# thin to square
thintosquare <- function(thin,nvert) {
    # create the return matrix
    matrix <- matrix(rep(0,nvert*nvert),nrow = nvert,ncol = nvert);
    
    # iterate through rows
    for(row in 1:nrow(thin)) {
        # get edge
        matrix[thin[row,1],thin[row,2]] <- 1;
    }
    
    return(matrix);
}