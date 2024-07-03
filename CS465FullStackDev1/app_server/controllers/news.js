const fs = require('fs');
const newsList = JSON.parse(fs.readFileSync('./data/newsList.json', 'utf8'));
const vacationTips = JSON.parse(fs.readFileSync('./data/vacationTips.json', 'utf8'));

const news = (req, res) => {
    pageTitle = 'Travlr Getaways - News';
    res.render('news', {title: pageTitle, newsList, vacationTips});
};

module.exports = {
    news
}