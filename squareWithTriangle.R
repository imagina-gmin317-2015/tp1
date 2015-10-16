motif<-matrix(c(0,0,0,1,1,0,0,1,1,0,1,1),nrow=6,byrow = TRUE)
xtimes<-4
ytimes<-4

init<-motif
final<-init
for (x in 1:(xtimes-1)){
  init[,1]<-init[,1]+1
  final<-rbind(final,init)
}

xline<-final
final<-xline
for (x in 1:(xtimes-1)){
  xline[,2]<-init[,2]+1
  final<-rbind(final,xline)
}

paste0(final[,1],",",final[,2],collapse = ",")
