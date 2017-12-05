<?php
//print "data!!!! : ".$_POST['data'];
$data_to_save = json_decode($_POST['data'], true);
//print $data_to_save;
if(file_exists ( "position.csv" ))
    $file = fopen("position.csv","a");
else
    $file = fopen("position.csv","w");
foreach ($data_to_save['position'] as $list)
{
    fputcsv($file,$list);
}
fclose($file);
echo "write CSV file complete.";
?>