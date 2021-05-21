console.log("xdd")
if (document.cookie.replace(/(?:(?:^|.*;\s*)loggedin\s*\=\s*([^;]*).*$)|^.*$/, "$1") == "True") {
	var id = document.cookie.replace(/(?:(?:^|.*;\s*)id\s*\=\s*([^;]*).*$)|^.*$/, "$1");
	var name = document.cookie.replace(/(?:(?:^|.*;\s*)name\s*\=\s*([^;]*).*$)|^.*$/, "$1");
	var img = document.cookie.replace(/(?:(?:^|.*;\s*)img\s*\=\s*([^;]*).*$)|^.*$/, "$1");
	var email = document.cookie.replace(/(?:(?:^|.*;\s*)email\s*\=\s*([^;]*).*$)|^.*$/, "$1");
	console.log(id + "xd")
}
else {
function onSignIn(googleUser) {
  var profile = googleUser.getBasicProfile();
  console.log('ID: ' + profile.getId()); // Do not send to your backend! Use an ID token instead.
  console.log('Name: ' + profile.getName());
  console.log('Image URL: ' + profile.getImageUrl());
  console.log('Email: ' + profile.getEmail()); // This is null if the 'email' scope is not present.
  document.cookie = "id=" + profile.getId() ;
  document.cookie = "name=" + profile.getName();
  document.cookie = "img=" + profile.getImageUrl() ;
  document.cookie = "email=" + profile.getEmail() ;
  document.cookie = "loggedin=True";
  console.log("ok")
}
}