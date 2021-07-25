<!--      Start the HTML Page with a comment         -->
<!--      Print out the title, etc. and opening tags --> 
<html>
<head><title>Single Page version of Add User</title></head>
<body>

<!--      Here comes the php portion of the page     -->
<?php

//------------------------------------------------------
// Now a php comment 
// This is the main program that either processes the form
// data or prints the form to capture data
//------------------------------------------------------
if (isset($_POST['stage']) && ($_POST['stage'] == 'process'))
{
   process_form();
}
else
{
   print_form();
}

//------------------------------------------------------
// This is the function that prints the form
//------------------------------------------------------
function print_form()
{
echo <<<END
    <center>
      <table width=70% cellpadding=5 border=1 bgcolor=#008080>
	<tr>
	    <td align=center> <font size=+3 style=times color=white>
		Become a Registered Customer</font><br>
	    </td>

	  </tr>
      </table>
    </center>
    
    <table border="0">
	<tr>
	  <form action="$_SERVER[PHP_SELF]" method="post">
	    
	  <td>Username: </td>
	  <td><input type="Text" name="username" align="TOP" size="20"></td>

	</tr>
	<tr>
	  <td>Password:</td>
	  <td><input type="Text" name="password" align="TOP" size="20"></td>
	</tr>
	<tr>
	  <td><input type="hidden" name="stage" value="process"</td>
	  <td><input type="Submit" name="submit" value="Add Me" align="MIDDLE"></td>
	</tr>

    </table>

    </form>
END;
}

//------------------------------------------------------
// This is the function that processes the form data
//------------------------------------------------------
function process_form()
{
   echo "<h2>Username: " . $_POST["username"] . "</h2></br>";
   echo "<h2>Password: " . $_POST["password"] . "</h2></br>";
}

?>
</body>
</html>
