# Kyle Dale
# CS-340-Client/Server Dev
# Professor Sanford
# Project One
# Crud Functionality Module
# 7/29 - 7/30
# Refactored 8/6/2023

from pymongo import MongoClient

# Defines the AnimalShelter class
class AnimalShelter(object):
    """ CRUD operations for Animal collection in MongoDB """

    def __init__(self, USER, PASS, HOST, PORT, DB, COL):
        # Initializing the MongoClient. This helps to 
        # access the MongoDB databases and collections.
        # This is hard-wired to use the aac database, the 
        # animals collection, and the aac user.
        # Definitions of the connection string variables are
        # unique to the individual Apporto environment.
        #
        # You must edit the connection variables below to reflect
        # your own instance of MongoDB!
        #
        # Connection Variables
        #
        self.USER = 'aacuser'
        self.PASS = 'CS340UserPass'
        self.HOST = 'nv-desktop-services.apporto.com'
        self.PORT = 32072
        self.DB = 'AAC'
        self.COL = 'animals'
        #
        # Initialize Connection
        #
        self.client = MongoClient('mongodb://%s:%s@%s:%d' % (self.USER,self.PASS,self.HOST,self.PORT))
        self.database = self.client['%s' % (self.DB)]
        self.collection = self.database['%s' % (self.COL)]

    # Method to implement the C in CRUD.
    def create(self, data):
        if data is not None:
            return self.collection.insert_one(data).inserted_id  # data should be a dictionary
        else:
            raise Exception("Nothing to save, because data parameter is empty")

    # Method to implement the R in CRUD.
    def read(self, query):
        result = []
        if query is not None:
            cursor = self.collection.find(query)
            for doc in cursor:
                result.append(doc)
        return result

    # Method to implement the U in CRUD (update one or many).
    # Method to implement update one.
    def update_one(self, query, update_data):
        if query is not None and update_data is not None:
            result = self.collection.update_one(query, {'$set': update_data})
            return result.modified_count
        else:
            raise Exception("Query and update_data parameters cannot be empty.")
    # Method to implement update many.
    def update_many(self, query, update_data):
        if query is not None and update_data is not None:
            result = self.collection.update_many(query, {'$set': update_data})
            return result.modified_count
        else:
            raise Exception("Query and update_data parameters cannot be empty.")
            
    # Method to implement the D in CRUD (delete one or many).
    # Method to implement delete one. 
    def delete_one(self, query):
        if query is not None:
            result = self.collection.delete_one(query)
            return result.deleted_count
        else:
            raise Exception("Query parameter cannot be empty.")
    # Method to implement delete many.
    def delete_many(self, query):
        if query is not None:
            result = self.collection.delete_many(query)
            return result.deleted_count
        else:
            raise Exception("Query parameter cannot be empty.")
