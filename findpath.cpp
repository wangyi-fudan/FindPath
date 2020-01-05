#include	<sys/time.h>
#include	<sys/mman.h>
#include	<sys/stat.h>
#include	<algorithm>
#include	<iostream>
#include	<stdint.h>
#include	<unistd.h>
#include	"wyhash.h"
#include	<cstdlib>
#include	<fstream>
#include	<sstream>
#include	<fcntl.h>
#include	<cfloat>
#include	<vector>
#include	<cmath>
#include	<omp.h>
using	namespace	std;

struct	Sort{
	unsigned	i,	n;
	bool	operator()(Sort	X,	Sort	Y){	return	X.n>Y.n;	}
};

class	findpath{
private:
	int	fd0,	fd1;
	struct	stat	sb0,	sb1;
	vector<string>	name;
	unsigned	*idx,	*data;
public:
	findpath(string	F){
		string	fn=F+".names";
		ifstream	fi(fn.c_str());
		string	s;
		for(fi>>s;	!fi.eof();	fi>>s)	name.push_back(s);
		fi.close();
		fn=F+".edges";
		fd0=open(fn.c_str(),	O_RDONLY);
		if(fd0<0)	return;
		fstat(fd0,	&sb0);
		data=(unsigned*)mmap(NULL,	sb0.st_size,	PROT_READ,	MAP_SHARED,	fd0,	0);
		if(data==MAP_FAILED)	return;
		mlock(data,	sb0.st_size);
		fn=F+".index";
		fd1=open(fn.c_str(),	O_RDONLY);
		if(fd1<0)	return;
		fstat(fd1,	&sb1);
		idx=(unsigned*)mmap(NULL,	sb1.st_size,	PROT_READ,	MAP_SHARED,	fd1,	0);
		if(idx==MAP_FAILED)	return;
		mlock(idx,	sb1.st_size);
		if((size_t)(sb1.st_size>>2)==name.size()+1)	cerr<<F<<'\t'<<name.size()<<'\n';
	}
	~findpath(){
		munlock(data,	sb0.st_size);	munlock(idx,	sb1.st_size);
		munmap(data,	sb0.st_size);	close(fd0);
		munmap(idx,		sb1.st_size);	close(fd1);
	}
	bool	find(string	A,	string	B,	size_t	D,	size_t	T){
		uint64_t	aid,	bid,	rng=wyhash(A.c_str(),	A.size(),time(NULL))^wyhash(B.c_str(),   B.size(),time(NULL));
		auto	vi=lower_bound(name.begin(),name.end(),A);
		if(vi==name.end()||*vi!=A){	cerr<<"fail to find "<<A<<'\n';	return	false;	}
		aid=vi-name.begin();
		vi=lower_bound(name.begin(),name.end(),B);
		if(vi==name.end()||*vi!=B){	cerr<<"fail to find "<<B<<'\n';	return	false;	}
		bid=vi-name.begin();
		vector<unsigned>	p,	bp;	float	mind=FLT_MAX;
		for(size_t	t=0;	t<T;	t++){
			uint64_t	c=aid;	p.clear();	p.push_back(c);	float	dis=0;
			for(size_t	d=0;	d<D;	d++){
				if(idx[c+1]==idx[c])	break;
				dis+=log1pf(idx[c+1]-idx[c]);
				c=data[idx[c]+wyrand(&rng)%(idx[c+1]-idx[c])];
				p.push_back(c);	
				if(c==bid){	if(dis<mind){	mind=dis;	bp=p;	}	break;	}
			}
		}
		for(auto	vj=bp.begin();	vj!=bp.end();	++vj)	cout<<name[*vj]<<'\t';
		cout<<'\n';
		return	true;
	}
};

void	document(void){
	cerr<<"Usage:	findpath [options] network A B\n";
	cerr<<"\t-d:	max search depth	default=5\n";
	cerr<<"\t-t:	million of trials	default=1\n";
	exit(0);
}

int	main(int	ac,	char	**av){
	int	opt;	size_t	depth=5,	trials=1<<20;
	while((opt=getopt(ac,	av,	"d:t:"))>=0) {
		switch(opt) {
		case	'd':	depth=atoi(optarg);	break;
		case	't':	trials=atoi(optarg)<<20;	break;
		default:	document();
		}
	}
	if(ac<optind+3)	document();
	findpath	fp(av[optind]);
	fp.find(av[optind+1],	av[optind+2],	depth,	trials);	
	return	0;
}
