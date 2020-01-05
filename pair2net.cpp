#include	"bytell_hash_map.hpp"
#include	<algorithm>
#include	<iostream>
#include	<fstream>
#include	<sstream>
#include	<vector>
using	namespace	std;

bool	load_pair(const	char	*F,	vector<string>	&V,	ska::bytell_hash_set<string>	&se){
	size_t	n=V.size();
	ifstream	fi(F);
	if(!fi){	cerr<<"fail to open "<<F<<'\n';	return	false;	}
	string	s,	a,	b;	
	while(getline(fi,s)){
		istringstream	si(s);
		si>>a>>b;
		se.insert(a);	se.insert(b);	V.push_back(a);	V.push_back(b);
	}
	fi.close();
	cerr<<F<<'\t'<<(V.size()-n)/2<<'\n';
	return	true;
}

int	main(int	ac,	char	**av){
	if(ac<3){	cerr<<"pair2net output in1.pair in2.pair ...\n";	return	0;	}
	vector<string>	v;	ska::bytell_hash_set<string>	se;
	for(int	i=2;	i<ac;	i++)	if(!load_pair(av[i],v,se))	return	0;
	vector<string>	u(se.begin(),se.end());	sort(u.begin(),u.end());
	ska::bytell_hash_map<string,	unsigned>	ma;
	string	fn=av[1];	fn+=".names";
	ofstream	fo(fn.c_str());
	for(size_t	i=0;	i<u.size();	i++){	ma[u[i]]=i;	fo<<u[i]<<'\n';	}
	fo.close();
	vector<uint64_t>	data;
	for(size_t	i=0;	i<v.size();	i+=2){
		uint64_t	a=ma[v[i]],	b=ma[v[i+1]];
		data.push_back((a<<32)|b);
		data.push_back((b<<32)|a);
	}
	sort(data.begin(),data.end());	data.resize(unique(data.begin(),data.end())-data.begin());
	fn=av[1];	fn+=".edges";	FILE	*f=fopen(fn.c_str(),	"wb");
	fn=av[1];	fn+=".index";	fo.open(fn.c_str(),	ios::binary);
	for(size_t	i=0;	i<data.size();){
		unsigned	off=ftell(f)>>2;
		fo.write((char*)&off,	4);
		size_t	j;
		for(j=i;	j<data.size()&&(data[i]>>32)==(data[j]>>32);	j++){
			off=data[j]&0xffffffff;	fwrite(&off,4,1,f);
		}
		i=j;
	}
	unsigned	off=ftell(f)>>2;
	fo.write((char*)&off,	4);
	fo.close();	fclose(f);
	cerr<<"\nTotal Nodes\t"<<se.size()<<'\n';
	cerr<<"Total Edges\t"<<data.size()/2<<'\n';
	return	0;
}
