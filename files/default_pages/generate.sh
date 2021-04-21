if [ "$#" -ne 2 ]
then
	echo 'Error : Need two args. Usage : ./generate error_code error_message'

else
	sed "s/{{error_code}}/$1/g; s/{{error_message}}/$2/g" ./custom_template.html > ./custom_$1.html
	echo "error_$1.html has been generated"
fi