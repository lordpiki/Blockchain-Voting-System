const letters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

let interval = null;

document.addEventListener("DOMContentLoaded", () => {
  
  const h2 = document.querySelector("h2");
  startRandomizeEffect(h2, h2.innerText);  
});

function startRandomizeEffect(targetElement, originalText) {
  let iteration = 0;
  
  clearInterval(interval);
  
  interval = setInterval(() => {
    targetElement.innerText = originalText
      .split("")
      .map((letter, index) => {
        if(index < iteration) {
          return originalText[index];
        }
      
        return letters[Math.floor(Math.random() * 26)];
      })
      .join("");
    
    if(iteration >= originalText.length){ 
      clearInterval(interval);
    }
    
    iteration += 1 / 4.5;
  }, 30);
}
