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
  x = vector("list", nvert)
  for(i in 1:nrow(thin))
  {
    
  }
  
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