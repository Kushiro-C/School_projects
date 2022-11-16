package com.example.mobile_project

import android.app.Application
import androidx.lifecycle.AndroidViewModel
import androidx.lifecycle.MutableLiveData

class ViewModel (application : Application) : AndroidViewModel(application) {
    private val dao = PlantDatabase.getDatabase(application).myDao()
    var certainsPlants = MutableLiveData<List<Plant>>()
    var toUpdate = MutableLiveData(0L)

    // Fill the List<Plant> with all Plants with a query
    init {
        val t = Thread {
            certainsPlants.postValue(dao.loadAll().toList())
        }
        t.start()
        t.join()
    }

    // Update List from query starting with 'nom'
    fun partialNomPlant(nom: String) {
        Thread {
            certainsPlants.postValue(dao.loadPartialName(nom).toList())
        }.start()
    }

    fun loadAll() {
        Thread {
            certainsPlants.postValue(dao.loadAll().toList())
        }.start()
    }

    // Inserts a Plant in our database through a query
    fun insertData(plant: Plant) {
        val t = Thread {
            val listLong = dao.insertPlant(plant)
            if (listLong.isNotEmpty()) {
                // Put Insert Result value (success or not)
                toUpdate.postValue(listLong[0])
            }
        }
        t.start()
        t.join()
    }

    // Update Plant in our database
    fun updateData(plant: Plant) {
        val t = Thread {
            dao.updatePlant(plant)
        }
        t.start()
        t.join()
    }

    fun deleteData(idPlant: Int) {
        val t = Thread {
            dao.deletePlant(idPlant)
        }
        t.start()
        t.join()
    }
}