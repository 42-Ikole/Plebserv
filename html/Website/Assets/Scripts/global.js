var profOpen
function myprofile() {
    if(profOpen == 1){
      document.getElementById("profileoverlay").style.height = "0px";
      document.getElementById("profileoverlay").style.display = "none";
      profOpen = 0;
    }
    else {
      document.getElementById("profileoverlay").style.display = "block";
      document.getElementById("profileoverlay").style.height = "250px";
      profOpen = 1;
    }
  }
function signOut() {
    var auth2 = gapi.auth2.getAuthInstance();
    auth2.signOut().then(function () {
      console.log('User signed out.');
      NewPage(001)
    });
  }