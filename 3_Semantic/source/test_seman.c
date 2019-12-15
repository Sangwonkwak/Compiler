int gcd(int u,int v,int x[]){
	if(v==0) return u;
	else return gcd(v,u-u/v*v);
	{ int x;int y; }
	/* asdasd */
}

void main(void){
	int x;int y;int z;
	z[3]=0;
	x = input(); y = input();
	output(gcd(x,y));
}
