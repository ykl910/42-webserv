    (async () => {
        const container = document.getElementById("dynamic-posts");
        const maxPosts = 100;

        for (let i = maxPosts - 1; i >= 0; i--) {
            try {
                const [titleResp, bodyResp] = await Promise.all([
                    fetch(`/threads/${i}_title.txt`),
                    fetch(`/threads/${i}_body.txt`)
                ]);

                if (!titleResp.ok || !bodyResp.ok) continue;

                const [title, body] = await Promise.all([
                    titleResp.text(),
                    bodyResp.text()
                ]);

                // Vérifie image .jpg ou .png
                let imgPath = `/threads/${i}_img.jpg`;
                let imgResp = await fetch(imgPath);
                if (!imgResp.ok) {
                    imgPath = `/threads/${i}_img.png`;
                    imgResp = await fetch(imgPath);
                    if (!imgResp.ok) continue;
                }

                const post = document.createElement("section");
                post.className = "user-post";
                post.innerHTML = `
                    <h2>${title.trim()}</h2>
                    <img src="${imgPath}" alt="Image du post ${i}">
                    <p>${body.trim()}</p>
                `;
                container.appendChild(post);
            } catch (e) {
                console.error("Erreur lors du chargement du post", i, e);
            }
        }
    })();
