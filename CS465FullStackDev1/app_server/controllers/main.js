const fs = require('fs');
const newsList = JSON.parse(fs.readFileSync('./data/newsList.json', 'utf8'));
const testimonials = JSON.parse(fs.readFileSync('./data/testimonials.json', 'utf8'));

const index = (req, res) => {
    res.render('index', {title: 'Travlr Getaways', newsList, testimonials});
};

module.exports = {
    index
}