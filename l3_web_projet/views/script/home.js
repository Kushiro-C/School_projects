$(document).ready(function() {

    var listPosts = []; // These 3 Lists
    var listMess = []; // have the same length
    var listAuthors = []; // for each publications
    var listUsers = [];
    var listSubs = [];

    // When checkboxes changes, update posts to display
    $('input:checkbox').on("change", "", function() {
        displayPosts();
    });

    /**
     * Matches matchList's elements with str
     * Matches listSubs as well, if one of the match is "abonnement"
     * Returns true if str contains one of the elements in the list
     * @param {String Array} matchList - elements to match
     * @param {String} str - String to search in
     */
    function matchListInString(matchList, str) {
        for (var i = 0; i < matchList.length; i++) {
            if ($(matchList[i]).val() == "abonnement") {
                for (var j = 0; j < listSubs.length; j++) {
                    if (str.indexOf(listSubs[j]) >= 0)
                        return true;
                }
            } else if (str.toLowerCase().indexOf($(matchList[i]).val().toLowerCase()) >= 0)
                return true;
        }
        return false;
    }

    /**
     * Displays post matching with one of the element in the checked checkboxes
     * We check if the author or the message of each post contains the checkbox value
     */
    function displayPosts() {
        var matchList = $('input:checkbox:checked');
        var matchSearch = $("#search_input");
        for (var i = 0; i < listAuthors.length; i++) {
            if (matchSearch.length != 0 && $(matchSearch).val().length > 0) { // Searching something precise
                if (matchListInString(matchSearch, listAuthors[i]) ||
                    matchListInString(matchSearch, listMess[i]))
                    $(listPosts[i]).show();
                else
                    $(listPosts[i]).hide();
            } else { // Not searching
                if (matchListInString(matchList, listAuthors[i]) ||
                    matchListInString(matchList, listMess[i]))
                    $(listPosts[i]).show();
                else
                    $(listPosts[i]).hide();
            }
        }
    }

    // do not allow users to enter spaces:
    $("#search_input").on({
        keydown: function(event) {
            if (event.which === 32)
                return false;
        },
        // if a space replace it
        change: function() {
            this.value = this.value.replace(/\s/g, '');
            displayPosts();
        }
    });

    /**
     * Send request to publish a new post
     */
    function publishPost() {
        var name = $('.userName').attr('id');
        var mess = $('#toSend').val();
        $.post('/publish', { name: name, mess: mess }, function() {});
        updatePage();
    }

    // Check text to publish (fil d'actualité)
    $('#publication').on('click', '.bouton_publier', function() {
        var hasInput = false;
        $('.checkvalid').each(function() {
            if ($(this).val() !== "" && /\S/.test($(this).val())) {
                hasInput = true;
            }
        });
        if (!hasInput)
            alert("need input");
        else
            publishPost();
    });

    /**
     * Update Like for a post and send request to server
     * @param {int} i - index
     */
    function updateLike(i) {
        var str = $('.like').eq(i).text();
        var newLikes = parseInt(str.substring(5, str.length));
        newLikes++;
        $('.like').eq(i).text('Like ' + newLikes);
        var id = $('.description').eq(i).attr('id');

        $.post('/upLike', { likes: newLikes, id: id }, function() {});
    }

    /**
     * Update Dislike for a post and send request to server
     * @param {int} i - index
     */
    function updateDislike(i) {
        var str = $('.dislike').eq(i).text();
        var newDislikes = parseInt(str.substring(8, str.length));
        newDislikes++;
        $('.dislike').eq(i).text('Dislike ' + newDislikes);
        var id = $('.description').eq(i).attr('id');

        $.post('/upDislike', { dislikes: newDislikes, id: id }, function() {});
    }

    // Action Click on Like button
    $('#publication').on('click', '.like', function() {
        var i = $('.like').index(this);
        updateLike(i);
    });

    // Action Click on Dislike button
    $('#publication').on('click', '.dislike', function() {
        var i = $('.dislike').index(this);
        updateDislike(i);
    });

    /**
     * AJAX Sending request to server to update the new posts
     */
    function updatePage() {
        // Add publications only if there are new
        var nbPubs = $(".bloc_post").length;
        $.get('/update', {}, function(data) {
            var users = data.listUsers;
            var subs = data.subscriptions;
            var pubs = data.publications;

            // Create and Get publications
            for (var i = pubs.length - 1 - nbPubs; i >= 0; i--) {
                var str = $(`
                    <div class="bloc_post">
                        <div zoomOn="${pubs[i].pub_id}>.jpg" id="${pubs[i].pub_id}.jpg"  class="myImgs">
                            <td><a class="photo"><img src="${pubs[i].nickname}.jpg" alt="Insert ${pubs[i].nickname}.jpg" width="250px" height="350px"></a></td>
                        </div>
                        <div class="description" id="${pubs[i].pub_id}">
                            <p class="author">Auteur : ${pubs[i].nickname}</p>
                            <p class="date">Posté le : ${new Date(pubs[i].date).toString('dd-MM-yy')}</p>
                            <p class="message">${pubs[i].message}</p>
                            <span class="like">Like ${pubs[i].nLike}</span>
                            <span class="dislike">Dislike ${pubs[i].nDislike}</span>
                        </div>
                    </div>
                `);
                listPosts[i] = str; // Get Publications Object
                listMess[i] = pubs[i].message; // Get Publications Messages
                listAuthors[i] = pubs[i].nickname; // Get Publications Authors
                $("#app").after(str); // Add Publication to Page
            }

            // Get Users nicknames
            var nbUsers = listUsers.length;
            for (var i = users.length - 1 - nbUsers; i >= 0; i--)
                listUsers[i] = users[i].nickname;

            // Get Subscriptions nicknames
            var nbSubs = listSubs.length;
            for (var i = subs.length - 1 - nbSubs; i >= 0; i--) {
                listSubs[i] = subs[i].followed_nickname;
                $("#abonnement").append(' ' + listSubs[i]);
            }

            // Show message if no subscriptions
            if (listSubs.length == 0)
                $("#abonnement").append(' Vide');

            displayPosts();
        });
    }

    updatePage();
    setInterval(updatePage, 5000);
});