# AdministracionServiciosRed
Programas desarrollados en administración de servicios en red 
comandos de git http://rogerdudler.github.io/git-guide/index.es.html

Git force pull to overwrite local files
git fetch --all
git reset --hard origin/master
git pull origin master

#Servidor DNS del politecnico 
148.204.103.2
#Problemas en git add 
cd .git/objects
ls -al

The output of ls -al command will look somthing like this

drwxrwxrwx 58 andrew833 staff 1972 Feb 4 00:24 .
drwxr-xr-x 15 andrew833 staff 510 Feb 4 00:34 ..
drwxr-xr-x 3 root staff 102 Feb 3 00:24 0c
drwxr-xr-x 3 andrew833 staff 102 Feb 3 01:40 0d
drwxr-xr-x 3 andrew833 staff 102 Feb 3 01:27 11
drwxr-xr-x 3 andrew833 staff 102 Feb 4 00:34 13
...
In my case andrew833 is my username and staff is my group. Now I changed the ‘root‘ username with my username ‘andrew833’ by running this command. Don’t forget the * at the end of the line.

sudo chown -R andrew833:staff *

---Iniciar con repositorio--
www.github.com/RicardoVargasSagrero
git clone url/repositorio
//Se empieza a trabajar
git add . //Agrega todos los archivos cambiados
git commit -m "Comentario" //hace el commit 
git push //Sube los cambios al repositorio

//Cuando inicies de nuevo a programar 
git status //checa el status del repositorio
git pull //Baja los ultimos cambios del repositorio