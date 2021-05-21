function NewPage(n) {
  var ok = n;
  var page = "";
  if(ok == "001") {var page = "Home.html"};
  if(ok == "002") {var page = "Inquerys.html"};
  if(ok == "003") {var page = "Account.html"};
  if(ok == "004") {var page = "Checkout.html"};
  if(ok == "005") {var page = "Profile.html"};
  if(ok == "006") {var page = "Developer.html"};
  if(ok == "009") {var page = "../Welcome.html"};
  if(ok == "018") {var page = "../Store/Login.html"};
  console.log(page)
  console.log(n)
  document.getElementById("body").style.opacity = "0";
  setTimeout(function (){
  window.open(page,"_self");
}, 700);


}
