m <- c(1,0,1,0,1,0,0,1,1,0,0,0,0,0,0,1)
ms <- matrix(m,4,4)

m <- c(1,1,1,3,4,4,1,2,3,1,2,4)
mt <- matrix(m,6,2)

ml <- list(1:3, NA, 1, c(2,4))


squaretolist <- function(mat) {
  l <- list()
  for(m in 1:nrow(mat)) {
    i <- 1
    for(n in 1:ncol(mat)) {
      if(mat[m,n] != 0) { #column-major form
        ifelse(i == 1, l[[m]] <- n, l[[m]][i] <- n)
        i = i+1
      }
    }
  }
  print(l)
}



squaretothin <- function(mat) {
  mm <- matrix(NA,0,2)
  i <- 1 
 for(m in 1:nrow(mat)) {
   for(n in 1:ncol(mat)) {
     if(mat[m,n] != 0) { #column-major form
       M <- matrix(c(m,n), 1, 2)
       mm = rbind(mm,M)
     }
   }
 }
 print(mm)
}



thintolist <- function(thin,nvert) {
  i <- 1
  j <- 1
  l <- list()
  for(n in 1:nrow(thin)) {
    while(i != thin[n,1]) {
      i = i+1
      j <- 1
    } 
    if(j==1) {
      l[[i]] <- thin[n,2]
      j = j+1
    }
    else {
      l[[i]][j] <- thin[n,2]
      j = j+1
    }
  }
  while(i < nvert) {
    i = i+1
    l[[i]] = NA
  }
  print(l)
}


thintosquare <- function(thin, nvert) {
  i <- 1
  j <- 1
  mm <- matrix(0,nvert,nvert)
  for(n in 1:nrow(thin)) {
    while(i != thin[n,1]) {
      i = i+1
      j <- 1
    } 
    mm[i,thin[n,2]] <- 1
    j = j+1
  }
  print(mm)
}



listtothin <- function(inlist) {
  mm <- matrix(NA,0,2)
  i <- 1
  for(n in 1:length(inlist)) {
    for(m in 1:length(inlist[[n]])) {
      z = inlist[[n]][m]
      if(!is.na(z)) {
        M <- matrix(c(n,z), 1, 2)
        mm = rbind(mm,M)        
      }
    }
  }
  print(mm)
}



listtosquare <- function(inlist) {
  mm <- matrix(0,4,4)
  i <- 1
  for(n in 1:length(inlist)) {
    for(m in 1:length(inlist[[n]])) {
      z = inlist[[n]][m]
      if(!is.na(z)) {
        mm[n,z] <- 1
      }
    }
  }
  print(mm) 
}
