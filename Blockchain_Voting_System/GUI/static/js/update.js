function updateStats() {
    fetch('/stats')
        .then(response => response.json())
        .then(data => {
            console.log(data);
            document.getElementById('stats').innerHTML = `
                <p>IS MINING: ${data["Miner Info"].isMining}</p>
                <p>VOTES IN QUEUE: ${data["Miner Info"].votesInQueue}</p>
                <p>BLOCK COUNT: ${data["Miner Info"].blockCount}</p>
            `;
            

            blocksInfo = data["Blocks Info"];
            document.querySelector('.blocks').innerHTML = '';
            // Create new blocks
            for (i in blocksInfo)
            {
                block = blocksInfo[i]
                const blockElement = document.createElement('div');
                const chainElement = document.createElement('div');

                chainElement.classList.add('chain-element');
                blockElement.classList.add('block');
                blockElement.innerHTML = `
                    <h3>BLOCK: ${block.Block}</h3>
                    <p>VOTES: ${block.Votes}</p>
                    <p>HASH: ${block.Hash.slice(0, 16)}</p>
                    <p>PREVIOUS HASH: ${block.PrevHash.slice(0, 16)}</p>
                `;
                document.querySelector('.blocks').appendChild(blockElement);
                document.querySelector('.blocks').appendChild(chainElement.cloneNode(true));
                document.querySelector('.blocks').appendChild(chainElement.cloneNode(true));
                document.querySelector('.blocks').appendChild(chainElement.cloneNode(true));

            }

            // Delete the last 3 chainElements
            const blocksContainer = document.querySelector('.blocks');
            const chainElements = blocksContainer.querySelectorAll('.chain-element');
            for (let i = chainElements.length - 1; i >= chainElements.length - 3; i--) {
                blocksContainer.removeChild(chainElements[i]);
            }
            
        });
}
