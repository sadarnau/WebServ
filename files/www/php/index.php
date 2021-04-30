<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>A simple php page</title>
</head>
<body>
    <h1>If you can see the date, php is working </h1>
    <h1><?php echo date('l jS \of F Y h:i:s A'); ?></h1>
    
    <h2>Args passed in query :</h2>
    <ul>
    <?php
        parse_str(getenv('QUERY_STRING'), $_GET);
        foreach($_GET as $key => $value){
            echo "<li>" . $key . " : " . $value . "</li>\n";
        }

    ?>
    </ul>
</body>
</html>