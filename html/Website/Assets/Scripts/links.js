function NewPage(n) {
  var ok = n;
  var page = "";
  if(ok == "001") {var page = "../Welcome.html"};
  if(ok == "002") {var page = "Login.html"};
  if(ok == "003") {var page = "Start.html"};
  if(ok == "004") {var page = "Checkout.html"};
  if(ok == "005") {var page = "../Coin/Home.html"};
  if(ok == "103") {var page = "../Store/Start.html"};


  document.getElementById("body").style.opacity = "0";
  setTimeout(function (){
  window.open(page,"_self");
}, 700);


}
