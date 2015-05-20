for i='N':'N'
name1='matlab_data11.txt';
name2='matlab_data21.txt';
name3='matlab_data31.txt';
path1='matlab_path11.txt';
path2='matlab_path21.txt';
path3='matlab_path31.txt';

name1(13)=i;
name2(13)=i;
name3(13)=i;
path1(13)=i;
path2(13)=i;
path3(13)=i;
name1
fid_data1=fopen(name1,'r');
fid_data2=fopen(name2,'r');
fid_data3=fopen(name3,'r');
fid_path1=fopen(path1,'r');
fid_path2=fopen(path2,'r');
fid_path3=fopen(path3,'r');
A=fscanf(fid_data1,'%lf',[2,inf]);
B=fscanf(fid_data2,'%lf',[2,inf]);
C=fscanf(fid_data3,'%lf',[2,inf]);
str1=textread(path1,'%s','delimiter','\n');
str2=textread(path2,'%s','delimiter','\n');
str3=textread(path3,'%s','delimiter','\n');
%plot(A(1,:),A(2,:),'o-','LineWidth',2','MarkerSize',6');
%plot(A(1,:),A(2,:),'o-',B(1,:),B(2,:),'o-','LineWidth',2','MarkerSize',8');
figure('Color',[1 1 1]);

all_data=fopen('actual_pos.txt','r');
D=fscanf(all_data,'%lf',[2,inf]);

plot(D(1,:),D(2,:),'o',A(1,:),A(2,:),'o-',B(1,:),B(2,:),'o-',C(1,:),C(2,:),'o-','LineWidth',2','MarkerSize',6');
xlabel('Longitude');
ylabel('Latitude');
%axis([-100 200 -100 200]);

str1={'Center','Center','Center','Siguiri','Kouroussa','Dabola','Kindia','Dubreka','Conakry','Coyah','Forecariah','Kambia','Kankan','Kissidougou','Gueckedou','Beyla','Kailahun','Kenema','Lola','Bonthe','Pujehun','Bomi','Nimba','Margibi','Grand Bassa','Rivercess County','Sinoe','Grand Kru','Faranah','Koinadugu','Kono','Kerouane','Moyamba','Bo','Macenta','Nz��r��kor��','Grand Cape Mount','Bong','Bombali','Port Loko','Tonkolili','Western area rural','Montserrado','Freetown'};
set(text(D(1,:),D(2,:)-0.0015,str1),'FontSize',9);
%text(A(1,:),A(2,:)-0.0015,str1);
%text(B(1,:),B(2,:)-0.0015,str2);
%text(C(1,:),C(2,:)-0.0015,str3);

%figure('Color',[1 1 1]);
fid_bestdis=fopen('best_dis.txt','r');
B=fscanf(fid_bestdis,'%lf',[1,inf]);
for i=1:50
    B(2,i)=i;
end
%plot(B(2,:),B(1,:),'-','LineWidth',2');
%ylabel('Fitness of Best Solution');
%xlabel('Generation');
end;