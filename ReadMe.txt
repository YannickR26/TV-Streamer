***************************
* Procédure d'instalation *
***************************

1. Utiliser Win_32_Disk_Imager pour formater et copier Raspbian-wheezy sur la carte SD.

2. Copier le fichier config.txt dans la partition Boot/

3. Modifier la configuration reseau de la carte Rpi. Fichier /etc/network/interfarce

4. Branché la Rpi, vérifié qu'elle boot correctement.

5. Connecter vous avec login: pi, mdp: raspberry

6. Lancé: sudo raspi-config
		sélectionner Expand FileSystem.
		Rebooter la Rpi

7. Lancé: sudo raspi-config
		Configurer la local et la langue.
		Rebooter la Rpi

8. Mis a jour de la carte: sudo apt-get update, sudo apt-get upgrade

9. Installer samba: sudo apt-get install samba

10. Copié (depuis le PC) le logiciel: scp Gestion_Tv_Fitness pi@<ip_address>:~

11. Installer PM2
		sudo apt-get install npm
		sudo npm install -g pm2

12. Creer le lanceur automatique
		sudo pm2 startup
		sudo pm2 Gestion_Tv_Fitness start
		sudo pm2 save

13. Terminer


***************************
* Pour Compiller sur RPI  *
***************************

1. Installer apt-transport-https
		sudo apt-get install apt-transport-https

2. Installer Qt4
		sudo apt-get install libqt4-dev

3. La rpi est prete pour compiler le soft

4. Depuis le pc
	installer sshfs
		sudo apt-get install sshfs

	installer qt4
		sudo apt-get install libqt4-dev-bin

	Creer le repertoire de montage de la RPI
		sudo mkdir /mnt/rpi

	Se connecter une premiere fois via sshfs
		sudo sshfs pi@192.168.1.25:/ /mnt/rpi

5. Le pc est prete pour compiler le soft
