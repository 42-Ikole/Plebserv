  function infoBox1() {
      document.getElementById("myNav1").style.width = "100%";
      document.getElementById("overlay-img1").style.height = "500px";  
  }

  function closeNav1() {
      document.getElementById("myNav1").style.width = "0%";
      document.getElementById("overlay-img1").style.height = "1px";  
  }

  function infoBox2() {
      document.getElementById("myNav2").style.width = "100%";
      document.getElementById("overlay-img2").style.height = "500px";      
  }

  function closeNav2() {
      document.getElementById("myNav2").style.width = "0%";
      document.getElementById("overlay-img2").style.height = "1px";  
  }
  function infoBox3() {
      document.getElementById("myNav3").style.width = "100%";
      document.getElementById("overlay-img3").style.height = "500px";  
  }

  function closeNav3() {
      document.getElementById("myNav3").style.width = "0%";
      document.getElementById("overlay-img3").style.height = "1px";  
  }
  function infoBox4() {
      document.getElementById("myNav4").style.width = "100%";
      document.getElementById("overlay-img4").style.height = "500px";  
  }

  function closeNav4() {
      document.getElementById("myNav4").style.width = "0%";
      document.getElementById("overlay-img4").style.height = "1px";  
  }

  var elem = document.getElementById("myBar");   
  var width = 0;
  var id = setInterval(frame, 20);
  function frame() {
    if (width >= 63) {
      clearInterval(id);
    } else {
      width++; 
      elem.style.width = width + '%'; 
      document.getElementById("demo").innerHTML = width * 1  + '/10/25/100/1000';
    }
  }
  function NewPage1() {
    document.getElementById("body").style.opacity = "0";
    setTimeout(function (){
    window.open("Login.html","_self");
    }, 700);
    }
  var slideIndex = 1;
  showSlides(slideIndex);

  function plusSlides(n) {
    showSlides(slideIndex += n);
  }

  function currentSlide(n) {
    showSlides(slideIndex = n);
  }

  function showSlides(n) {
    var i;
    var slides = document.getElementsByClassName("wrapper");
    var dots = document.getElementsByClassName("dot");
    if (n > slides.length) {slideIndex = 1}    
    if (n < 1) {slideIndex = slides.length}
    for (i = 0; i < slides.length; i++) {
        slides[i].style.display = "none";  
    }
    for (i = 0; i < dots.length; i++) {
        dots[i].className = dots[i].className.replace(" active", "");
    }
    slides[slideIndex-1].style.display = "block";  
    dots[slideIndex-1].className += " active";
  }