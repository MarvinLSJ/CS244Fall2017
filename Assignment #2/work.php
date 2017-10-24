<html>
	<head>
		<style>
			table, th, td {
			    border: 1px solid black;
			}
		</style>
	</head>

	<body>
	<?php
		$data_array = array();
		if(!is_null($_GET['num']) and !is_null($_GET['IR']) and !is_null($_GET['RED'])) {
			$file = fopen('file.csv', 'r'); 
			if($file != FALSE) {
				while (($data = fgetcsv($file, 1000, ",")) != False) {
					array_push($data_array, $data);
				}
			}
			fclose($file);

			$file = fopen('file.csv', 'w');
			if (isset($data_array[$_GET['num']]) and sizeof($data_array[$_GET['num']]) > 0) {
				array_push($data_array[$_GET['num']], $_GET['IR'], $_GET['RED']);
			} else {
				$input_data = array($_GET['num'], $_GET['IR'], $_GET['RED']);
				array_push($data_array, $input_data);
			}
			
			foreach ($data_array as $each) {
				fputcsv($file, $each);
			}
			fclose($file);
		}
		$file = fopen("file.csv", "r");
		if ($file != FALSE) {
			echo "<table><tr><th>num</th><th>IR</th><th>RED</th></tr>";
		    while (($data = fgetcsv($file, 1000, ",")) != FALSE) {
		        $num = count($data);
				echo "<tr>";
		        for ($c=0; $c < $num; $c++) {
		            echo "<td>" . $data[$c] . "</td>";
		        }
				echo "</tr>";
		    }
			echo "</table>";
		    fclose($file);
		}
	?>
	</body>
</html>