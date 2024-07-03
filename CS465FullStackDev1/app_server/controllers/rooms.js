const fs = require('fs');
const room = JSON.parse(fs.readFileSync('./data/room.json', 'utf8'));

const rooms = (req, res) => {
    pageTitle = 'Travlr Getaways - Rooms';
    res.render('rooms', {title: pageTitle, room});
};

module.exports = {
    rooms
}